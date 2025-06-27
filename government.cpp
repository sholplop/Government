#include <iostream>
#include <vector>
#include <string>
#include "test.h"

class GovernmentProject;

class ProjectAction {
public:
    virtual void execute(GovernmentProject &project) = 0;
    virtual ~ProjectAction() = default;
};

class GovernmentProject {
    std::string project_name;
    std::string department;
    bool is_funded;
    double budget;
    bool is_completed;
    std::vector<ProjectAction*> actions;

public:
    GovernmentProject(const std::string &name, const std::string &dept,
                     bool funded, double budget_amount,
                     const std::vector<ProjectAction*> &acts)
        : project_name(name), department(dept),
          is_funded(funded), budget(budget_amount),
          is_completed(false), actions(acts) {}

    std::string getProjectName() const { return project_name; }
    std::string getDepartment() const { return department; }
    bool isFunded() const { return is_funded; }
    double getBudget() const { return budget; }
    bool isCompleted() const { return is_completed; }

    void setFunded(bool funded) { is_funded = funded; }
    void setBudget(double amount) { budget = amount; }
    void setCompleted(bool completed) { is_completed = completed; }
    void setDepartment(const std::string &dept) { department = dept; }

    void process() {
        for (auto *action : actions) {
            action->execute(*this);
        }
    }

    ~GovernmentProject() {
        for (auto *action : actions) {
            delete action;
        }
    }
};

class ApproveFunding : public ProjectAction {
public:
    void execute(GovernmentProject &project) override {
        project.setFunded(true);
    }
};

class AdjustBudget : public ProjectAction {
    double adjustment;
public:
    AdjustBudget(double adj) : adjustment(adj) {}
    void execute(GovernmentProject &project) override {
        project.setBudget(project.getBudget() + adjustment);
    }
};

class CompleteProject : public ProjectAction {
public:
    void execute(GovernmentProject &project) override {
        if (project.isFunded()) {
            project.setCompleted(true);
        }
    }
};

class ConditionalApproval : public ProjectAction {
    ProjectAction* action;
    double min_budget;
public:
    ConditionalApproval(ProjectAction* act, double budget)
        : action(act), min_budget(budget) {}
    void execute(GovernmentProject &project) override {
        if (project.getBudget() >= min_budget) {
            action->execute(project);
        }
    }
    ~ConditionalApproval() { delete action; }
};

class DepartmentTransfer : public ProjectAction {
    std::string new_department;
public:
    DepartmentTransfer(const std::string &dept) : new_department(dept) {}
    void execute(GovernmentProject &project) override {
        project.setDepartment(new_department);
    }
};

class BudgetFreeze : public ProjectAction {
public:
    void execute(GovernmentProject &project) override {
        project.setBudget(0);
    }
};

class ProjectRegistry {
    std::vector<GovernmentProject*> projects;
public:
    void addProject(GovernmentProject *project) { projects.push_back(project); }

    void processAll() {
        for (auto *project : projects) {
            project->process();
        }
    }

    ~ProjectRegistry() {
        for (auto *project : projects) {
            delete project;
        }
    }
};

TEST(GovernmentTest, InfrastructureProjectApproval) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new ApproveFunding(), new AdjustBudget(500000) };
    GovernmentProject* bridge = new GovernmentProject("River Bridge", "Transportation", false, 1000000, actions);
    registry.addProject(bridge);
    registry.processAll();
    ASSERT_TRUE(bridge->isFunded());
    ASSERT_EQ(bridge->getBudget(), 1500000);
    return true;
}

TEST(GovernmentTest, EducationBudgetCut) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new AdjustBudget(-200000) };
    GovernmentProject* schools = new GovernmentProject("School Upgrade", "Education", true, 800000, actions);
    registry.addProject(schools);
    registry.processAll();
    ASSERT_EQ(schools->getBudget(), 600000);
    return true;
}

TEST(GovernmentTest, ProjectCompletionWorkflow) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new CompleteProject() };
    GovernmentProject* hospital = new GovernmentProject("City Hospital", "Health", true, 2000000, actions);
    registry.addProject(hospital);
    registry.processAll();
    ASSERT_TRUE(hospital->isCompleted());
    return true;
}

TEST(GovernmentTest, ConditionalBudgetApproval) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new ConditionalApproval(new ApproveFunding(), 1000000) };
    GovernmentProject* highway = new GovernmentProject("Highway Expansion", "Transportation", false, 1200000, actions);
    registry.addProject(highway);
    registry.processAll();
    ASSERT_TRUE(highway->isFunded());
    return true;
}

TEST(GovernmentTest, InsufficientBudgetRejection) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new ConditionalApproval(new ApproveFunding(), 5000000) };
    GovernmentProject* airport = new GovernmentProject("Airport Renovation", "Transportation", false, 3000000, actions);
    registry.addProject(airport);
    registry.processAll();
    ASSERT_TRUE(!airport->isFunded());
    return true;
}

TEST(GovernmentTest, MultiActionProject) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = {
        new ApproveFunding(),
        new AdjustBudget(750000),
        new CompleteProject()
    };
    GovernmentProject* library = new GovernmentProject("Central Library", "Culture", false, 1250000, actions);
    registry.addProject(library);
    registry.processAll();
    ASSERT_TRUE(library->isFunded());
    ASSERT_EQ(library->getBudget(), 2000000);
    ASSERT_TRUE(library->isCompleted());
    return true;
}

TEST(GovernmentTest, DepartmentTransfer) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new DepartmentTransfer("Urban Development") };
    GovernmentProject* park = new GovernmentProject("City Park", "Environment", true, 500000, actions);
    registry.addProject(park);
    registry.processAll();
    ASSERT_EQ(park->getDepartment(), "Urban Development");
    return true;
}

TEST(GovernmentTest, BudgetFreezeAction) {
    ProjectRegistry registry;
    std::vector<ProjectAction*> actions = { new BudgetFreeze() };
    GovernmentProject* museum = new GovernmentProject("National Museum", "Culture", true, 3000000, actions);
    registry.addProject(museum);
    registry.processAll();
    ASSERT_EQ(museum->getBudget(), 0);
    return true;
}

int main() {
    RUN_TEST(GovernmentTest, InfrastructureProjectApproval);
    RUN_TEST(GovernmentTest, EducationBudgetCut);
    RUN_TEST(GovernmentTest, ProjectCompletionWorkflow);
    RUN_TEST(GovernmentTest, ConditionalBudgetApproval);
    RUN_TEST(GovernmentTest, InsufficientBudgetRejection);
    RUN_TEST(GovernmentTest, MultiActionProject);
    RUN_TEST(GovernmentTest, DepartmentTransfer);
    RUN_TEST(GovernmentTest, BudgetFreezeAction);
    return 0;
}
