# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################
import bokeh.plotting
import bokeh.models
import bokeh.layouts
import bokeh.palettes

import argparse
import numpy as np

###############################################################################


def visualize_trend(trend_data, instance_name, output_file_name):
    colors = bokeh.palettes.brewer['YlGnBu'][4]

    TOOLTIPS = [
        ("index", "$index"),
        ("(x,y)", "($x, $y)"),
    ]

    # Elapsed Time
    fig_elapsed_time = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Elapse Time',
        x_axis_label='Iteration',
        y_axis_label='Elapsed Time[s]',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig_elapsed_time.line(
        x=trend_data[:, 0],
        y=trend_data[:, 1],
        legend='Elapsed Time',
        width=3,
        color=colors[0])

    fig_elapsed_time.legend.visible = False

    # intensity
    fig_intensity = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Intensity',
        x_axis_label='Iteration',
        y_axis_label='Intensity',
        x_range=bokeh.models.DataRange1d(start=0),
        y_axis_type="log",
        plot_width=500,
        plot_height=300)

    fig_intensity.line(
        x=trend_data[:, 0],
        y=trend_data[:, 6],
        legend='',
        width=3,
        color=colors[0])

    fig_intensity.legend.visible = False

    # objective
    fig_objective = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Objective',
        x_axis_label='Iteration',
        y_axis_label='Objective',
        x_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig_objective.circle(
        x=trend_data[:, 0],
        y=trend_data[:, 2],
        legend='Local Incumbent',
        size=3,
        color=colors[1],
        fill_alpha=0.8)

    fig_objective.line(
        x=trend_data[:, 0],
        y=trend_data[:, 4],
        legend='Global Incumbent',
        width=3,
        color=colors[0])

    fig_objective.legend.visible = True
    fig_objective.legend.location = "bottom_center"
    fig_objective.legend.orientation = "horizontal"
    new_legend = fig_objective.legend[0]
    fig_objective.add_layout(new_legend, 'below')

    # violation
    fig_violation = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Violation',
        x_axis_label='Iteration',
        y_axis_label='Violation',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig_violation.circle(
        x=trend_data[:, 0],
        y=trend_data[:, 3],
        legend='Local Incumbent',
        size=3,
        color=colors[1],
        fill_alpha=0.8)

    fig_violation.line(
        x=trend_data[:, 0],
        y=trend_data[:, 5],
        legend='Global Incumbent',
        width=3,
        color=colors[0])

    fig_violation.legend.visible = True
    fig_violation.legend.location = "bottom_center"
    fig_violation.legend.orientation = "horizontal"
    new_legend = fig_violation.legend[0]
    fig_violation.add_layout(new_legend, 'below')

    # Penalty Coefficient Control
    fig_penalty_coefficient_control = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Penalty Coefficient Control',
        x_axis_label='Iteration',
        y_axis_label='Ratio',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig_penalty_coefficient_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 11]) / (trend_data[:, 0]+1),
        legend='Relaxing',
        width=3,
        color=colors[1])

    fig_penalty_coefficient_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 12]) / (trend_data[:, 0]+1),
        legend='Tightening',
        width=3,
        color=colors[0])

    fig_penalty_coefficient_control.line(
        x=trend_data[:, 0],
        y=1.0 - np.cumsum(trend_data[:, 11] +
                          trend_data[:, 12]) / (trend_data[:, 0]+1),
        legend='No Updating',
        width=3,
        color=colors[2])

    fig_penalty_coefficient_control.legend.visible = True
    fig_penalty_coefficient_control.legend.location = "bottom_center"
    fig_penalty_coefficient_control.legend.orientation = "horizontal"
    new_legend = fig_penalty_coefficient_control.legend[0]
    fig_penalty_coefficient_control.add_layout(new_legend, 'below')

    # Penalty Coefficient Relaxing/Tightening Rate Control
    fig_penalty_coefficient_rate_control = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Penalty Coefficient Relaxing/Tightening Rate Control',
        x_axis_label='Iteration',
        y_axis_label='Value',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig_penalty_coefficient_rate_control.line(
        x=trend_data[:, 0],
        y=trend_data[:, 14],
        legend='Relaxing',
        width=3,
        color=colors[1])

    fig_penalty_coefficient_rate_control.line(
        x=trend_data[:, 0],
        y=trend_data[:, 15],
        legend='Tightening',
        width=3,
        color=colors[0])

    fig_penalty_coefficient_rate_control.legend.visible = True
    fig_penalty_coefficient_rate_control.legend.location = "bottom_center"
    fig_penalty_coefficient_rate_control.legend.orientation = "horizontal"
    new_legend = fig_penalty_coefficient_rate_control.legend[0]
    fig_penalty_coefficient_rate_control.add_layout(new_legend, 'below')

    # Penalty Coefficient Reset Control
    fig_penalty_coefficient_reset_control = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Penalty Coefficient Reset Control',
        x_axis_label='Iteration',
        y_axis_label='Ratio',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig_penalty_coefficient_reset_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 13]) / (trend_data[:, 0]+1),
        legend='',
        width=3,
        color=colors[0])

    fig_penalty_coefficient_reset_control.legend.visible = False

    # Initial Solution Control
    fig_initial_solution_control = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Initial Solution Control',
        x_axis_label='Iteration',
        y_axis_label='Ratio',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig_initial_solution_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 8]) / (trend_data[:, 0]+1),
        legend='Local Incumbent',
        width=3,
        color=colors[1])

    fig_initial_solution_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 9]) / (trend_data[:, 0] + 1),
        legend='Global Incumbent',
        width=3,
        color=colors[0])

    fig_initial_solution_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 10]) / (trend_data[:, 0] + 1),
        legend='Same as Previous',
        width=3,
        color=colors[2])

    fig_initial_solution_control.legend.visible = True
    fig_initial_solution_control.legend.location = "bottom_center"
    fig_initial_solution_control.legend.orientation = "horizontal"
    new_legend = fig_initial_solution_control.legend[0]
    fig_initial_solution_control.add_layout(new_legend, 'below')

    # Initial Modification Control
    fig_initial_modification_control = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Initial Modification Control',
        x_axis_label='Iteration',
        y_axis_label='Ratio',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0.0, end=1.1),
        plot_width=500,
        plot_height=300)

    fig_initial_modification_control.line(
        x=trend_data[:, 0],
        y=np.cumsum(trend_data[:, 16]) / (trend_data[:, 0]+1),
        legend='',
        width=3,
        color=colors[0])

    fig_initial_modification_control.legend.visible = False

    # Initial Tabu Tenure Control
    fig_initial_tabu_tenure = bokeh.plotting.figure(
        tooltips=TOOLTIPS,
        title='Initial Tabu Tenure Control',
        x_axis_label='Iteration',
        y_axis_label='Initial Tabu Tenure',
        x_range=bokeh.models.DataRange1d(start=0),
        y_range=bokeh.models.DataRange1d(start=0),
        plot_width=500,
        plot_height=300)

    fig_initial_tabu_tenure.line(
        x=trend_data[:, 0],
        y=trend_data[:, 18],
        legend='',
        width=3,
        color=colors[0])

    fig_initial_tabu_tenure.legend.visible = False
    fig_initial_tabu_tenure.yaxis.ticker\
        = bokeh.models.tickers.SingleIntervalTicker(interval=10)

    # plot
    grid = bokeh.layouts.gridplot(
        [[fig_elapsed_time, fig_intensity],
         [fig_objective, fig_violation],
         [fig_penalty_coefficient_control, fig_penalty_coefficient_rate_control],
         [fig_penalty_coefficient_reset_control, fig_initial_solution_control],
         [fig_initial_modification_control, fig_initial_tabu_tenure]])

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
    trend_data = np.loadtxt(trend_file_name)

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
