# -*- coding: utf-8 -*-
import json
import sys
import numpy as np
import copy
import matplotlib.pyplot as plt

###############################################################################


def compute_distance(x, y):
    return np.abs(x - y).sum()

###############################################################################


def visualize_distance(solution_object, max_number_of_solutions=5000):
    # Summary
    name = solution_object['name']
    number_of_variables = solution_object['number_of_variables']
    number_of_constraints = solution_object['number_of_constraints']

    # Copy and sort raw solutions.
    raw_solutions = copy.deepcopy(solution_object['solutions'])
    raw_solutions.sort(key=lambda x: x['objective'])
    for solution in raw_solutions:
        solution['variables'] = np.array(solution['variables'])

    number_of_raw_solutions = len(raw_solutions)

    # Dedplication of solutions.
    unique_solutions = []
    EPSILON = 1E-5

    for i in range(number_of_raw_solutions):
        is_unique = True
        for j in range(i+1, number_of_raw_solutions):
            if raw_solutions[i]['objective'] != raw_solutions[j]['objective']:
                break
            if compute_distance(raw_solutions[i]['variables'],
                                raw_solutions[j]['variables']) < EPSILON:
                is_unique = False
                break

        if is_unique:
            unique_solutions.append(raw_solutions[i])
            if len(unique_solutions) == max_number_of_solutions:
                break

    number_of_unique_solutions = len(unique_solutions)

    # Compute Manhattan distances for each solutions pairs.
    distances = np.zeros(
        (number_of_unique_solutions, number_of_unique_solutions))

    for i in range(number_of_unique_solutions):
        for j in range(i+1, number_of_unique_solutions):
            distance = compute_distance(
                unique_solutions[i]['variables'],
                unique_solutions[j]['variables'])
            distances[i, j] = distance
            distances[j, i] = distance

    # Plot the Manhattan distances as a heatmap.
    title = 'Manhattan distance between 2 solutions \n %s (#Var.:%d, #Cons.:%d)' \
        % (name, number_of_variables, number_of_constraints)

    plt.title(title)
    plt.imshow(distances, interpolation='gaussian')
    plt.xlabel('Solution No.')
    plt.ylabel('Solution No.')
    plt.grid(False)
    plt.colorbar()
    plt.show()

###############################################################################


def main():
    solution_file_name = sys.argv[1]
    with open(solution_file_name, 'r') as f:
        solution_object = json.load(f)

    visualize_distance(solution_object)

###############################################################################


if __name__ == "__main__":
    main()

###############################################################################
# END
###############################################################################
