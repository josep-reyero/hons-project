#include "logical_solver_run.hpp"

void LogicalSolverRun::testOnTestProblem() {
  std::string test_problem =
      "/Users/pepe/hons-project/problems/presolve_test_problem2.txt";
  deprecated::Reader reader;
  // Start filestream to pass to reader.
  std::fstream problems_filestream;
  problems_filestream.open(test_problem, std::ios::in);

  reader.readNextProblem(problems_filestream);

  logical_solver::Presolve presolve(
      reader.problem_matrix_, reader.lower_bounds_, reader.upper_bounds_,
      reader.num_inequalities_, reader.num_equalities_, false);
  problems_filestream.close();

  presolve.printLP();
  presolve.applyPresolve();
  presolve.applyPostsolve();
  if (presolve.reduced_to_empty_) {
    printf("Reduced to empty\n");
  }
  if (!presolve.infeasible_) {
    presolve.printFeasibleSolution();
    if (presolve.isFeasibleSolutionInteger()) {
      std::cout << "Interger solution" << std::endl;
    }
  } else {
    std::cout << "Infeasible" << std::endl;
  }
  // presolve.printImpliedBounds();
}

void LogicalSolverRun::testOnMultipleProblems(
    const int problems_count, const std::string problems_filepath) {
  deprecated::Reader reader;

  // Start filestream to pass to reader.
  std::fstream problems_filestream;
  problems_filestream.open(problems_filepath, std::ios::in);

  int reduced_to_empty_count = 0;
  int infeasible_count = 0;
  int integer_feasible_count = 0;
  int infeasible_by_parallel_rows_count = 0;
  int unsatisfied_constraints = 0;
  int empty_problems = 0;

  for (int n = 0; n < problems_count; n++) {
    // printf("Solving problem %d\n", n);

    // Reading problem.
    reader.readNextProblem(problems_filestream);

    if (reader.problem_matrix_.size() > 0) {
      logical_solver::Presolve presolve(
          reader.problem_matrix_, reader.lower_bounds_, reader.upper_bounds_,
          reader.num_inequalities_, reader.num_equalities_, false);

      presolve.applyPresolve();
      presolve.applyPostsolve();

      if (presolve.unsatisfied_constraints_) {
        unsatisfied_constraints += 1;
      } else if (presolve.infeasible_) {
        if (presolve.infeasible_by_PR_) {
          infeasible_by_parallel_rows_count += 1;
        }
        infeasible_count += 1;
      } else if (presolve.reduced_to_empty_) {
        reduced_to_empty_count += 1;
        if (presolve.isFeasibleSolutionInteger()) {
          integer_feasible_count += 1;
        }
      }
    } else {
      empty_problems += 1;
    }
  }
  printf("%d NON-EMPTY PROBLEMS WERE REDUCED TO EMPTY AND A FEASIBLE SOLUTION "
         "WAS FOUND.\n",
         reduced_to_empty_count);
  printf("%d Problems had an integer feasible solution. \n",
         integer_feasible_count);
  printf("%d Problems were empty\n", empty_problems);
  printf("%d Problems were not feasible. \n", infeasible_count);
  printf("%d Problems were not feasible due to infeasible parallel rows. \n",
         infeasible_by_parallel_rows_count);
  printf("%d Problems led to unsatisfied constraints.\n",
         unsatisfied_constraints);
}

void LogicalSolverRun::testOnFullTestSet(const std::string problems_filepath) {
  const int all_test_cases_count = 150218;
  auto start = std::chrono::high_resolution_clock::now();
  testOnMultipleProblems(all_test_cases_count, problems_filepath);
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  long long seconds =
      std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  printf("Time taken (seconds): %d \n", int(seconds));
}