# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020-2021 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################
from venv import create
import bokeh.plotting
import bokeh.models
import bokeh.layouts
import bokeh.palettes

import argparse
import numpy as np
import pandas as pd

colors = bokeh.palettes.brewer['YlGnBu'][4]

TOOLTIPS = [
    ("index", "$index"),
    ("(x,y)", "($x, $y)"),
]


###############################################################################

def create_elapsed_time_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Elapse Time',
        x_axis_label='Iteration',
        y_axis_label='Elapsed Time[s]',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['elapsed_time'],
        legend_label='Elapsed Time',
        width=3,
        color=colors[0])

    fig.legend.visible = False
    return fig

###############################################################################


def create_intensity_chart(trend_data):
    iterations_update = trend_data['#iteration'][trend_data['update_status'] >= 3]
    update_markers = trend_data['primal_intensity'][iterations_update]

    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Intensity',
        x_axis_label='Iteration',
        y_axis_label='Intensity',
        x_range=bokeh.models.DataRange1d(start=0),
        y_axis_type="log",
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['primal_intensity'],
        legend_label='primal',
        width=3,
        color=colors[0])

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['dual_intensity'],
        legend_label='dual',
        width=3,
        color=colors[1])

    fig.scatter(
        x=iterations_update,
        y=update_markers,
        size=3,
        color='red',
        fill_alpha=0.8)

    fig.legend.visible = True
    return fig

###############################################################################


def create_averaged_inner_iteration_speed_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Averaged Inner Iteration Speed',
        x_axis_label='Iteration',
        y_axis_label='iterations / sec',
        x_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['averaged_inner_iteration_speed'],
        legend_label='',
        width=3,
        color=colors[0])

    fig.legend.visible = False
    return fig

###############################################################################


def create_averaged_move_evaluation_speed_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Averaged Move Evaluation Speed',
        x_axis_label='Iteration',
        y_axis_label='moves / sec',
        x_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['averaged_move_evaluation_speed'],
        legend_label='',
        width=3,
        color=colors[0])

    fig.legend.visible = False
    return fig


def create_objective_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Objective',
        x_axis_label='Iteration',
        y_axis_label='Objective',
        x_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig.circle(
        x=trend_data['#iteration'],
        y=trend_data['local_objective'],
        legend_label='Local Incumbent',
        size=3,
        color=colors[1],
        fill_alpha=0.8)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['global_objective'],
        legend_label='Global Incumbent',
        width=3,
        color=colors[0])

    fig.legend.visible = True
    fig.legend.location = "bottom_center"
    fig.legend.orientation = "horizontal"
    new_legend = fig.legend[0]
    fig.add_layout(new_legend, 'below')
    return fig

###############################################################################


def create_violation_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Violation',
        x_axis_label='Iteration',
        y_axis_label='Violation',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig.circle(
        x=trend_data['#iteration'],
        y=trend_data['local_violation'],
        legend_label='Local Incumbent',
        size=3,
        color=colors[1],
        fill_alpha=0.8)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['global_violation'],
        legend_label='Global Incumbent',
        width=3,
        color=colors[0])

    fig.legend.visible = True
    fig.legend.location = "bottom_center"
    fig.legend.orientation = "horizontal"
    new_legend = fig.legend[0]
    fig.add_layout(new_legend, 'below')
    return fig

###############################################################################


def create_penalty_coefficient_control_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Penalty Coefficient Control',
        x_axis_label='Iteration',
        y_axis_label='Rate',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(
            trend_data['is_enabled_penalty_coefficient_relaxing']) /
        (trend_data['#iteration']+1),
        legend_label='Relaxing',
        width=3,
        color=colors[0])

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(
            trend_data['is_enabled_penalty_coefficient_tightening']) /
        (trend_data['#iteration']+1),
        legend_label='Tightening',
        width=3,
        color=colors[1])

    fig.line(
        x=trend_data['#iteration'],
        y=1.0 - np.cumsum(trend_data['is_enabled_penalty_coefficient_relaxing'] +
                          trend_data['is_enabled_penalty_coefficient_tightening'])
        / (trend_data['#iteration']+1),
        legend_label='No Updating',
        width=3,
        color=colors[2])

    fig.legend.visible = True
    fig.legend.location = "bottom_center"
    fig.legend.orientation = "horizontal"
    new_legend = fig.legend[0]
    fig.add_layout(new_legend, 'below')
    return fig

###############################################################################


def create_penalty_coefficient_rate_control_chart(trend_data):
    iterations_update = trend_data['#iteration'][trend_data['update_status'] >= 3]
    update_marker_relax = trend_data['penalty_coefficient_relaxing_rate'][
        iterations_update[trend_data['is_enabled_penalty_coefficient_relaxing'] == 0]]
    update_markers_tighten = trend_data['penalty_coefficient_tightening_rate'][
        iterations_update[trend_data['is_enabled_penalty_coefficient_tightening'] == 0]]

    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Penalty Coefficient Relaxing/Tightening Rate Control',
        x_axis_label='Iteration',
        y_axis_label='Value',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['penalty_coefficient_relaxing_rate'],
        legend_label='Relaxing',
        width=3,
        color=colors[1])

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['penalty_coefficient_tightening_rate'],
        legend_label='Tightening',
        width=3,
        color=colors[0])

    fig.scatter(
        x=iterations_update[trend_data['is_enabled_penalty_coefficient_relaxing'] == 0],
        y=update_marker_relax,
        size=3,
        color='red',
        fill_alpha=0.8)

    fig.scatter(
        x=iterations_update[trend_data['is_enabled_penalty_coefficient_tightening'] == 0],
        y=update_markers_tighten,
        size=3,
        color='red',
        fill_alpha=0.8)

    fig.legend.visible = True
    fig.legend.location = "bottom_center"
    fig.legend.orientation = "horizontal"
    new_legend = fig.legend[0]
    fig.add_layout(new_legend, 'below')
    return fig

###############################################################################


def create_penalty_coefficient_reset_control_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Penalty Coefficient Reset Control',
        x_axis_label='Iteration',
        y_axis_label='Rate',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(trend_data['penalty_coefficient_reset_flag'])
        / (trend_data['#iteration']+1),
        legend_label='',
        width=3,
        color=colors[0])

    fig.legend.visible = False
    return fig

###############################################################################


def create_initial_solution_control_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Initial Solution Control',
        x_axis_label='Iteration',
        y_axis_label='Rate',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(trend_data['employing_local_augmented_solution_flag']
                    ) / (trend_data['#iteration'] + 1),
        legend_label='Local Incumbent',
        width=3,
        color=colors[1])

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(trend_data['employing_global_augmented_solution_flag']
                    ) / (trend_data['#iteration'] + 1),
        legend_label='Global Incumbent',
        width=3,
        color=colors[0])

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(trend_data['employing_previous_solution_flag']
                    ) / (trend_data['#iteration'] + 1),
        legend_label='Same as Previous',
        width=3,
        color=colors[2])

    fig.legend.visible = True
    fig.legend.location = "bottom_center"
    fig.legend.orientation = "horizontal"
    new_legend = fig.legend[0]
    fig.add_layout(new_legend, 'below')
    return fig

###############################################################################


def create_initial_modification_control_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Initial Modification Control',
        x_axis_label='Iteration',
        y_axis_label='Rate',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(
            trend_data['is_enabled_forcibly_initial_modification']) / (trend_data['#iteration']+1),
        legend_label='',
        width=3,
        color=colors[0])

    fig.legend.visible = False
    return fig

###############################################################################


def create_initial_tabu_tenure_control_chart(trend_data):
    iterations_update = trend_data['#iteration'][trend_data['update_status'] >= 3]
    update_markers = trend_data['initial_tabu_tenure'][iterations_update]

    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Initial Tabu Tenure Control',
        x_axis_label='Iteration',
        y_axis_label='Initial Tabu Tenure',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['initial_tabu_tenure'],
        legend_label='',
        width=3,
        color=colors[0])

    fig.legend.visible = False
    fig.yaxis.ticker\
        = bokeh.models.tickers.SingleIntervalTicker(interval=10)

    fig.scatter(
        x=iterations_update,
        y=update_markers,
        size=3,
        color='red',
        fill_alpha=0.8)
    return fig


###############################################################################


def create_performance_chart(trend_data):
    iterations_update = trend_data['#iteration'][trend_data['update_status'] >= 3]
    update_markers = trend_data['performance'][iterations_update]

    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Performance',
        x_axis_label='Iteration',
        y_axis_label='Performance',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=trend_data['performance'],
        legend_label='',
        width=3,
        color=colors[0])

    fig.scatter(
        x=iterations_update,
        y=update_markers,
        size=3,
        color='red',
        fill_alpha=0.8)

    fig.legend.visible = False
    return fig

###############################################################################


def create_parallelization_control_chart(trend_data):
    fig = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Parallelization Control',
        x_axis_label='Iteration',
        y_axis_label='Rate',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(trend_data['is_enabled_parallel_neighborhood_update']
                    ) / (trend_data['#iteration'] + 1),
        legend_label='Neighborhood Update',
        width=3,
        color=colors[0])

    fig.line(
        x=trend_data['#iteration'],
        y=np.cumsum(trend_data['is_enabled_parallel_evaluation']
                    ) / (trend_data['#iteration'] + 1),
        legend_label='Evaluation',
        width=3,
        color=colors[1])

    fig.legend.visible = True
    fig.legend.location = "bottom_center"
    fig.legend.orientation = "horizontal"
    new_legend = fig.legend[0]
    fig.add_layout(new_legend, 'below')
    return fig


###############################################################################


def visualize_trend(trend_data, instance_name, output_file_name):
    # Elapsed Time
    fig_elapsed_time = create_elapsed_time_chart(trend_data)

    # Intensity
    fig_intensity = create_intensity_chart(trend_data)

    # Averaged Inner Iteration Speed
    fig_averaged_inner_iteration_speed = create_averaged_inner_iteration_speed_chart(
        trend_data)

    # Averaged Move Evaluation Speed
    fig_averaged_move_evaluation_speed = create_averaged_move_evaluation_speed_chart(
        trend_data)

    # objective
    fig_objective = create_objective_chart(trend_data)

    # violation
    fig_violation = create_violation_chart(trend_data)

    # Penalty Coefficient Control
    fig_penalty_coefficient_control \
        = create_penalty_coefficient_control_chart(trend_data)

    # Penalty Coefficient Relaxing/Tightening Rate Control
    fig_penalty_coefficient_rate_control \
        = create_penalty_coefficient_rate_control_chart(trend_data)

    # Penalty Coefficient Reset Control
    fig_penalty_coefficient_reset_control \
        = create_penalty_coefficient_reset_control_chart(trend_data)

    # Initial Solution Control
    fig_initial_solution_control \
        = create_initial_solution_control_chart(trend_data)

    # Initial Modification Control
    fig_initial_modification_control \
        = create_initial_modification_control_chart(trend_data)

    # Initial Tabu Tenure Control
    fig_initial_tabu_tenure_control \
        = create_initial_tabu_tenure_control_chart(trend_data)

    # Performance
    fig_performance \
        = create_performance_chart(trend_data)

    # Parallelization Control
    fig_parallelization_control \
        = create_parallelization_control_chart(trend_data)

    # plot
    grid = bokeh.layouts.gridplot(
        [[fig_elapsed_time, fig_intensity],
         [fig_averaged_inner_iteration_speed, fig_averaged_move_evaluation_speed],
         [fig_objective, fig_violation],
         [fig_penalty_coefficient_control, fig_penalty_coefficient_rate_control],
         [fig_penalty_coefficient_reset_control, fig_initial_solution_control],
         [fig_initial_modification_control, fig_initial_tabu_tenure_control],
         [fig_performance, fig_parallelization_control]])

    bokeh.plotting.output_file(
        output_file_name,
        title='Tabu Search Trend for ' + instance_name)
    bokeh.plotting.save(grid)


###############################################################################
def main():
    parser = argparse.ArgumentParser(
        description='A script for visualizing tabu search trend.')
    parser.add_argument('input_file_name',
                        help='specify the input file name.',
                        type=str)
    parser.add_argument('-o', '--output',
                        help='specify the output file name.',
                        type=str,
                        default='trend.html')
    args = parser.parse_args()

    trend_file_name = args.input_file_name

    trend_data = pd.read_table(
        trend_file_name, sep=' ', header=0, skiprows=(0, 1, 2))

    with open(trend_file_name, 'r') as f:
        instance_name = f.readline().split()[-1]
        f.close()

    visualize_trend(trend_data, instance_name,
                    output_file_name=args.output)

###############################################################################


if __name__ == "__main__":
    main()

###############################################################################
# END
###############################################################################
