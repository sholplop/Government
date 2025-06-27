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
    std::vector<ProjectAction*> actions;

public:
    GovernmentProject(const std::string &name, const std::string &dept,
                     bool funded, double budget_amount,
                     const std::vector<ProjectAction*> &acts)
        : project_name(name), department(dept),
          is_funded(funded), budget(budget_amount), actions(acts) {}

    std::string getProjectName() const { return project_name; }
    std::string getDepartment() const { return department; }
    bool isFunded() const { return is_funded; }
    double getBudget() const { return budget; }

    void setFunded(bool funded) { is_funded = funded; }
    void setBudget(double amount) { budget = amount; }

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

int main() {
    RUN_TEST(GovernmentTest, InfrastructureProjectApproval);
    RUN_TEST(GovernmentTest, EducationBudgetCut);
    return 0;
}