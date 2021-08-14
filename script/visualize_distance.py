# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020-2021 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################

import json
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import networkx as nx

###############################################################################


def compute_distance(first, second):
    '''
    Compute the Manhattan distance between solutions first and second.
    '''
    first_set = set(first.keys())
    second_set = set(second.keys())
    only_first = first_set - second_set
    only_second = second_set - first_set
    intersection = first_set & second_set

    distance = 0
    for key in only_first:
        distance += abs(first[key])
    for key in only_second:
        distance += abs(second[key])
    for key in intersection:
        distance += abs(first[key] - second[key])
    return distance

###############################################################################


def visualize_distance(solution_object, output_file_name,
                       max_number_of_solutions, is_descending, is_enabled_create_mst):
    '''
    Visualize Manhattan distance for each solutions pair as a heatmap.
    '''
    # Summary
    name = solution_object['name']
    number_of_variables = solution_object['number_of_variables']
    number_of_constraints = solution_object['number_of_constraints']

    # Sort solutions.
    solutions = solution_object['solutions']
    if is_descending:
        solutions.sort(key=lambda x: -x['objective'])
    else:
        solutions.sort(key=lambda x: x['objective'])

    number_of_solutions = min(len(solutions), max_number_of_solutions)

    # Compute Manhattan distance for each solutions pair.
    distances = np.zeros(
        (number_of_solutions, number_of_solutions))

    for i in range(number_of_solutions):
        for j in range(i+1, number_of_solutions):
            distance = compute_distance(
                solutions[i]['variables'],
                solutions[j]['variables'])
            distances[i, j] = distance
            distances[j, i] = distance

    # Plot the Manhattan distances as a heatmap.
    title = 'Manhattan distance between 2 solutions \n (Instance: %s, #Var.: %d, #Cons.: %d)' \
        % (name, number_of_variables, number_of_constraints)
    if is_descending:
        footnote_text = '* The solutions are sorted in descending order of objective function value.'
    else:
        footnote_text = '* The solutions are sorted in ascending order of objective function value.'

    plt.title(title)
    plt.imshow(distances)
    plt.xlabel('Solution No.')
    plt.ylabel('Solution No.')
    plt.text(-0.1 * number_of_solutions,
             1.19 * number_of_solutions,
             footnote_text)

    plt.grid(False)
    plt.colorbar()
    plt.subplots_adjust(bottom=0.15)
    plt.savefig(output_file_name)

    # Create the minimum spanning tree (Optional)
    if is_enabled_create_mst:
        graph = nx.Graph()
        label = 'Solution network: The minimum spanning tree of a complete graph where nodes denote solutions. \\n' \
            + 'An edge connecting 2 nodes is weighted by Manhattan distance between the solutions. \\n' \
            + '(Instance: %s, #Var.: %d, #Cons.: %d)' \
            % (name, number_of_variables, number_of_constraints)
        graph.graph['graph'] = {
            'label': label,
            'labelloc': 't'}
        edges = []
        for i in range(number_of_solutions):
            for j in range(i+1, number_of_solutions):
                edges.append((i, j, distances[i, j]))

        graph.add_weighted_edges_from(edges)
        mst = nx.minimum_spanning_tree(graph)

        objectives = [solution['objective'] for solution in solutions]
        min_objective = np.min(objectives)
        max_objective = np.max(objectives)

        for i in range(number_of_solutions):
            color = cm.summer(
                np.log(objectives[i] - min_objective+1) / np.log(max_objective - min_objective+1))
            red = int(np.floor(color[0] * 255))
            green = int(np.floor(color[1] * 255))
            blue = int(np.floor(color[2] * 255))
            alpha = 127
            fillcolor = '#' + format(red, '02x') + format(green, '02x') + \
                format(blue, '02x') + format(alpha, '02x')

            mst.nodes[i]['label'] = objectives[i]
            mst.nodes[i]['penwidth'] = 0.1
            mst.nodes[i]['fillcolor'] = fillcolor
            mst.nodes[i]['style'] = 'filled, rounded'

        for (i, j) in mst.edges:
            mst.edges[i, j]['penwidth'] = 0.3

        nx.drawing.nx_agraph.write_dot(mst, 'distance.dot')

###############################################################################


def main():
    parser = argparse.ArgumentParser(
        description='A script for visualizing distance for each solutions pair.')
    parser.add_argument('input_file_name',
                        help='specify the input file name.',
                        type=str)
    parser.add_argument('-o', '--output',
                        help='specify the output file name.',
                        type=str,
                        default='distance.png')
    parser.add_argument('-s', '--size',
                        help='specify the maximum number of solutions for plot.',
                        type=int,
                        default=1000000)
    parser.add_argument('--descending',
                        help='sort solutions in descending order',
                        action='store_true')
    parser.add_argument('--dot',
                        help='enable generating the minimum spanning tree dot file',
                        action='store_true')
    args = parser.parse_args()

    solution_file_name = args.input_file_name
    with open(solution_file_name, 'r') as f:
        solution_object = json.load(f)
        f.close()

    visualize_distance(solution_object,
                       output_file_name=args.output,
                       max_number_of_solutions=args.size,
                       is_descending=args.descending,
                       is_enabled_create_mst=args.dot)

###############################################################################


if __name__ == "__main__":
    main()

###############################################################################
# END
###############################################################################
