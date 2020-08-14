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


def visualize(a_raw_solutions):
    # Copy and sort raw solutions.
    raw_solutions = copy.deepcopy(a_raw_solutions)
    raw_solutions.sort(key=lambda x: x['objective'])
    for solution in raw_solutions:
        solution['variables'] = np.array(solution['variables'])

    number_of_raw_solutions = len(raw_solutions)

    # Dedplication of solutions.
    unique_solutions = []
    EPSILON = 1E-5
    MAX_NUMBER_OF_SOLUTIONS = 5000

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
            if len(unique_solutions) == MAX_NUMBER_OF_SOLUTIONS:
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

    number_of_variables = len(unique_solutions[0]['variables'])

    # Plot the Manhattan distances as a heatmap.
    plt.imshow(distances, interpolation='gaussian')
    plt.title('Manhattan distance between 2 solutions (#Variable = %d)' %
              number_of_variables)
    plt.xlabel('Solution No.')
    plt.ylabel('Solution No.')
    plt.grid(False)
    plt.colorbar()
    plt.show()

###############################################################################


def main():
    solution_file_name = sys.argv[1]
    with open(solution_file_name, 'r') as f:
        solution = json.load(f)

    visualize(solution)

###############################################################################


if __name__ == "__main__":
    main()

###############################################################################
# END
###############################################################################
