# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################

import json
import argparse
import numpy as np
import copy
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import networkx as nx

###############################################################################


def compute_distance(x, y):
    '''
    Compute the Manhattan distance between solutions x and y.
    '''
    return np.abs(x - y).sum()

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

    # Copy and sort raw solutions.
    raw_solutions = copy.deepcopy(solution_object['solutions'])
    if is_descending:
        raw_solutions.sort(key=lambda x: -x['objective'])
    else:
        raw_solutions.sort(key=lambda x: x['objective'])

    for solution in raw_solutions:
        solution['variables'] = np.array(solution['variables'])

    number_of_raw_solutions = len(raw_solutions)

    # Dedeplication of solutions.
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

    # Compute Manhattan distance for each solutions pair.
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
    title = 'Manhattan distance between 2 solutions \n %s (#Var.: %d, #Cons.: %d)' \
        % (name, number_of_variables, number_of_constraints)
    if is_descending:
        footnote_text = '* The solutions are sorted in descending order of objective function value.'
    else:
        footnote_text = '* The solutions are sorted in ascending order of objective function value.'

    plt.title(title)
    plt.imshow(distances)
    plt.xlabel('Solution No.')
    plt.ylabel('Solution No.')
    plt.text(-0.1 * number_of_unique_solutions,
             1.19 * number_of_unique_solutions,
             footnote_text)

    plt.grid(False)
    plt.colorbar()
    plt.subplots_adjust(bottom=0.15)
    plt.savefig(output_file_name)

    # Create the minimum spanning tree (Optional)
    if is_enabled_create_mst:
        graph = nx.Graph()
        edges = []
        for i in range(number_of_unique_solutions):
            for j in range(i+1, number_of_unique_solutions):
                edges.append((i, j, distances[i, j]))

        graph.add_weighted_edges_from(edges)
        mst = nx.minimum_spanning_tree(graph)

        objectives = [solution['objective'] for solution in unique_solutions]
        min_objective = np.min(objectives)
        max_objective = np.max(objectives)

        for i in range(number_of_unique_solutions):
            color = cm.summer(
                np.sqrt(objectives[i] - min_objective) / np.sqrt(max_objective - min_objective))
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
        # nx.nx_agraph.view_pygraphviz(mst, prog='sfdp')


###############################################################################


def main():
    parser = argparse.ArgumentParser(
        description='A script for visualizing distance for each solutions pair.')
    parser.add_argument('input_file_name',
                        help='input file name.',
                        type=str)
    parser.add_argument('-o', '--output',
                        help='output file name.',
                        type=str,
                        default='distance.png')
    parser.add_argument('-s', '--size',
                        help='maximum number of solutions for plot.',
                        type=int,
                        default=5000)
    parser.add_argument('--descending',
                        help='Sort solution in descending order',
                        action='store_true')
    parser.add_argument('--dot',
                        help='Enable generate minimum spanning tree dot file',
                        action='store_true')
    args = parser.parse_args()

    solution_file_name = args.input_file_name
    with open(solution_file_name, 'r') as f:
        solution_object = json.load(f)

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
