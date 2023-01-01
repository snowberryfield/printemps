# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020-2023 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################

import copy
import json
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.cm as cm
import networkx as nx

from sklearn.manifold import MDS
from sklearn.manifold import TSNE

import pandas as pd

import plotly
import plotly.express as px

###############################################################################

np.random.seed(1)


def compute_distance(first, second):
    # Compute the Manhattan distance between solutions first and second.
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


class Visualizer:
    def __init__(self,
                 solution_object,
                 max_number_of_solutions,
                 is_enabled_shuffle,
                 is_descending):
        """
        Parameters
        ----------
            solution_object : dict
            max_number_of_solutions : int
            is_enabled_shuffle : bool
            is_descending : bool
        """
        self.solutions = copy.deepcopy(solution_object['solutions'])
        self.name = solution_object['name']
        self.number_of_variables = solution_object['number_of_variables']
        self.number_of_constraints = solution_object['number_of_constraints']

        self.max_number_of_solutions = max_number_of_solutions
        self.is_descending = is_descending
        self.number_of_solutions = min(
            len(self.solutions), self.max_number_of_solutions)

        print(' instance name: {}'.format(self.name))
        print(' number of variables: {}'.format(self.number_of_variables))
        print(' number of constraints: {}'.format(self.number_of_constraints))
        print(' original number of feasible solutions: {}'.format(
            len(self.solutions)))
        print(' reduced number of feasible solutions: {}'.format(
            self.number_of_solutions))

        if is_enabled_shuffle:
            np.random.shuffle(self.solutions)
            self.solutions = self.solutions[0:self.number_of_solutions]

        if self.is_descending:
            self.solutions.sort(key=lambda x: -x['objective'])
        else:
            self.solutions.sort(key=lambda x: x['objective'])
        self.solutions = self.solutions[0:self.number_of_solutions]

        self.__setup_distance_matrix()

    ###########################################################################
    def __setup_distance_matrix(self):
        # Compute Manhattan distance for each solutions pair.
        distance_matrix = np.zeros(
            (self.number_of_solutions, self.number_of_solutions))

        for i in range(self.number_of_solutions):
            for j in range(i+1, self.number_of_solutions):
                distance = compute_distance(
                    self.solutions[i]['variables'],
                    self.solutions[j]['variables'])
                distance_matrix[i, j] = distance
                distance_matrix[j, i] = distance
        self.distance_matrix = distance_matrix

    ###########################################################################
    def plot_heatmap(self,
                     output_file_name='heatmap.png',
                     is_enabled_write_title=False):
        # plot the Manhattan distance_matrix as a heatmap.
        title = 'Manhattan distance between 2 solutions \n (Instance: %s, #Variable: %d, #Constraint: %d)' \
            % (self.name, self.number_of_variables, self.number_of_constraints)
        if self.is_descending:
            footnote_text = '* The solutions are sorted in descending order of objective function value.'
        else:
            footnote_text = '* The solutions are sorted in ascending order of objective function value.'

        if is_enabled_write_title:
            plt.title(title)
        plt.imshow(self.distance_matrix)
        plt.xlabel('Solution No.')
        plt.ylabel('Solution No.')
        plt.text(-0.1 * self.number_of_solutions,
                 1.19 * self.number_of_solutions,
                 footnote_text)

        plt.grid(False)
        plt.colorbar()
        plt.subplots_adjust(bottom=0.15)
        plt.savefig(output_file_name)

    ###########################################################################
    def plot_tsne(self,
                  output_file_name='tsne.png',
                  is_enabled_write_title=False):
        # Plot the 2D solution coordinates mapped by t-SNE and objective function values.
        title = '2D solution coordinates mapped into 2D with t-SNE and objective function values\n (Instance: %s, #Variable: %d, #Constraint: %d)' \
            % (self.name, self.number_of_variables, self.number_of_constraints)

        tsne = TSNE(
            n_components=2, random_state=0)
        coordinates = tsne.fit_transform(self.distance_matrix)

        objectives = [self.solutions[i]['objective']
                      for i in range(self.number_of_solutions)]

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        if is_enabled_write_title:
            ax.set_title(title)
        ax.set_xlabel('t-SNE 1')
        ax.set_ylabel('t-SNE 2')
        ax.set_zlabel('Objective')

        def fmt(x, pos):
            a, b = '{:.2e}'.format(x).split('e')
            b = int(b)
            return r'${} \times 10^{{{}}}$'.format(a, b)

        ax.zaxis.set_major_formatter(ticker.FuncFormatter(fmt))
        scatter = ax.scatter(coordinates[:, 0],
                             coordinates[:, 1],
                             objectives,
                             vmin=np.min(objectives),
                             vmax=np.max(objectives),
                             c=objectives,
                             cmap=plt.get_cmap('hsv'),
                             marker='.')

        fig.colorbar(scatter, ax=ax, shrink=0.5,
                     aspect=8, pad=0.18, format=ticker.FuncFormatter(fmt))
        ax.zaxis.labelpad = 15
        plt.savefig(output_file_name)

    ###########################################################################
    def plot_mds(self,
                 output_file_name='mds.png',
                 is_enabled_write_title=False):
        # Plot the 2D solution coordinates mapped by MDS and objective function values.
        title = '2D solution coordinates mapped by MDS and objective function values\n (Instance: %s, #Variable: %d, #Constraint: %d)' \
            % (self.name, self.number_of_variables, self.number_of_constraints)

        mds = MDS(n_components=2, random_state=0, dissimilarity='precomputed')
        coordinates = mds.fit_transform(self.distance_matrix)

        objectives = [self.solutions[i]['objective']
                      for i in range(self.number_of_solutions)]

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        if is_enabled_write_title:
            ax.set_title(title)
        ax.set_xlabel('MDS 1')
        ax.set_ylabel('MDS 2')
        ax.set_zlabel('Objective')

        def fmt(x, pos):
            a, b = '{:.2e}'.format(x).split('e')
            b = int(b)
            return r'${} \times 10^{{{}}}$'.format(a, b)

        ax.zaxis.set_major_formatter(ticker.FuncFormatter(fmt))
        scatter = ax.scatter(coordinates[:, 0],
                             coordinates[:, 1],
                             objectives,
                             vmin=np.min(objectives),
                             vmax=np.max(objectives),
                             c=objectives,
                             cmap=plt.get_cmap('hsv'),
                             marker='.')

        fig.colorbar(scatter, ax=ax, shrink=0.5,
                     aspect=8, pad=0.18, format=ticker.FuncFormatter(fmt))
        ax.zaxis.labelpad = 15
        plt.savefig(output_file_name)

    ###########################################################################
    def plot_contribution(self,
                          output_file_name='contribution.html',
                          is_enabled_write_title=False):
        # Plot the variable contributions to objective function values.
        title = 'Variable contributions to objective function values\n (Instance: %s, #Variable: %d, #Constraint: %d)' \
            % (self.name, self.number_of_variables, self.number_of_constraints)

        objective_distribution_dict = {}

        for solution in self.solutions:
            for variable_name in solution['variables'].keys():
                if objective_distribution_dict.get(variable_name):
                    objective_distribution_dict[variable_name].append(
                        solution['objective'])
                else:
                    objective_distribution_dict[variable_name] = [
                        solution['objective']]

        objective_distributions = [
            {"variable": key, "objectives": value}
            for (key, value) in objective_distribution_dict.items()]

        objective_distributions.sort(key=lambda x: min(x['objectives']))

        cols = ['variable', 'objective']
        df = pd.DataFrame(index=[], columns=cols)
        for objective_distribution in objective_distributions:
            for objective in objective_distribution['objectives']:
                record = pd.Series(
                    [objective_distribution['variable'], objective], index=df.columns)
                df = df.append(record, ignore_index=True)

        fig = px.scatter(df,
                         x='objective',
                         y='variable',
                         orientation='h')

        fig.update_layout(
            title=title if is_enabled_write_title else '',
            autosize=True
        )

        plotly.offline.plot(fig, filename=output_file_name)

    ###########################################################################

    def write_minimum_spanning_tree_dot(self,
                                        output_file_name='mst.dot',
                                        is_enabled_write_title=False):
        # Create the minimum spanning tree and write as a dot file.
        graph = nx.Graph()
        label = 'Solution network: The minimum spanning tree of a complete graph where nodes denote solutions. \\n' \
            + 'An edge connecting 2 nodes is weighted by Manhattan distance between the solutions. \\n' \
            + '(Instance: %s, #Variable: %d, #Constraint: %d)' \
            % (self.name, self.number_of_variables, self.number_of_constraints)
        graph.graph['graph'] = {
            'label': label if is_enabled_write_title else '',
            'labelloc': 't'}
        edges = []
        for i in range(self.number_of_solutions):
            for j in range(i+1, self.number_of_solutions):
                edges.append((i, j, self.distance_matrix[i, j]))

        graph.add_weighted_edges_from(edges)
        mst = nx.minimum_spanning_tree(graph)

        objectives = [solution['objective'] for solution in self.solutions]
        min_objective = np.min(objectives)
        max_objective = np.max(objectives)

        for i in range(self.number_of_solutions):
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

        nx.drawing.nx_agraph.write_dot(mst, output_file_name)


###############################################################################


def main():
    parser = argparse.ArgumentParser(
        description='A script for visualizing feasible solutions.')
    parser.add_argument('input_file_name',
                        help='specify the input file name.',
                        type=str)
    parser.add_argument('-n', '--number_of_solutions',
                        help='specify the maximum number of solutions for plot.',
                        type=int,
                        default=1000000)
    parser.add_argument('-s', '--shuffle',
                        help='Extract solutions with shuffling if -n option is enabled.',
                        action='store_true')
    parser.add_argument('--descending',
                        help='sort solutions in descending order.',
                        action='store_true')
    parser.add_argument('--tsne',
                        help='enable generating 3D plot of 2D solution coordinates mapped by t-SNE and objective function values.',
                        action='store_true')
    parser.add_argument('--mds',
                        help='enable generating 3D plot of 2D solution coordinates mapped by MDS and objective function values.',
                        action='store_true')
    parser.add_argument('--contribution',
                        help='enable generating 2D plots of variable contributions to objective function values.',
                        action='store_true')
    parser.add_argument('--dot',
                        help='enable generating the minimum spanning tree dot file.',
                        action='store_true')
    parser.add_argument('--title',
                        help='add title to figures.',
                        action='store_true')
    args = parser.parse_args()

    solution_file_name = args.input_file_name
    with open(solution_file_name, 'r') as f:
        solution_object = json.load(f)
        f.close()

    visualizer = Visualizer(solution_object,
                            max_number_of_solutions=args.number_of_solutions,
                            is_enabled_shuffle=args.shuffle,
                            is_descending=args.descending)

    visualizer.plot_heatmap(is_enabled_write_title=args.title)

    if args.tsne:
        visualizer.plot_tsne(is_enabled_write_title=args.title)

    if args.mds:
        visualizer.plot_mds(is_enabled_write_title=args.title)

    if args.contribution:
        visualizer.plot_contribution(is_enabled_write_title=args.title)

    if args.dot:
        visualizer.write_minimum_spanning_tree_dot()


###############################################################################

if __name__ == '__main__':
    main()

###############################################################################
# END
###############################################################################
