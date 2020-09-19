# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################

import subprocess
import json
import argparse

###############################################################################


def run_benchmark(executable, mps_list, option_file_name,
                  is_enabled_separate_equality):

    if(is_enabled_separate_equality):
        separate_switch = '--separate'
    else:
        separate_switch = ''

    results = []

    for mps in mps_list:
        print('Solving' + mps + '...')
        subprocess.run([executable, mps, '-p',
                        option_file_name, separate_switch])
        with open('status.json', 'r') as f:
            status = json.load(f)
        with open('incumbent.json', 'r') as f:
            incumbent = json.load(f)

        result = {
            'name': status['name'],
            'number_of_variables': status['number_of_variables'],
            'number_of_constraints': status['number_of_constraints'],
            'is_found_feasible_solution': status['is_found_feasible_solution'],
            'elapsed_time': status['elapsed_time'],
            'number_of_lagrange_dual_iterations': status['number_of_lagrange_dual_iterations'],
            'number_of_local_search_iterations': status['number_of_local_search_iterations'],
            'number_of_tabu_search_iterations': status['number_of_tabu_search_iterations'],
            'number_of_tabu_search_loops': status['number_of_tabu_search_loops'],
            'objective': incumbent['objective']
        }
        results.append(result)
        print('n.variables:%d, n.constraints:%d, is_feasible:%d, objective:%f, elapsed time[sec]:%f' % (
            result['number_of_variables'],
            result['number_of_constraints'],
            result['is_found_feasible_solution'],
            result['objective'],
            result['elapsed_time']
        ))

    return results

###############################################################################


def write_csv(results):
    with open('benchmark_result.csv', 'w') as f:

        f.write(
            'name,number_of_variables,'
            + 'number_of_constraints,'
            + 'is_found_feasible_solution,'
            + 'objective,'
            + 'elapsed_time,'
            + 'number_of_lagrange_dual_iterations,'
            + 'number_of_local_search_iterations,'
            + 'number_of_tabu_search_iterations,'
            + 'number_of_tabu_search_loops\n')

        for result in results:
            f.write('%s,%d,%d,%d,%f,%f,%d,%d,%d,%d\n' % (
                result['name'],
                result['number_of_variables'],
                result['number_of_constraints'],
                result['is_found_feasible_solution'],
                result['objective'],
                result['elapsed_time'],
                result['number_of_lagrange_dual_iterations'],
                result['number_of_local_search_iterations'],
                result['number_of_tabu_search_iterations'],
                result['number_of_tabu_search_loops']
            ))


###############################################################################


def main():
    parser = argparse.ArgumentParser(
        description='A script for benchmarking of cpp_metaheuristics')
    parser.add_argument('executable',
                        help='specify the executable of cpp_metaheuristics.',
                        type=str)
    parser.add_argument('mps_list',
                        help='specify the mps list file name.',
                        type=str)
    parser.add_argument('-p', '--option',
                        help='specify the option file name.',
                        type=str)
    parser.add_argument('--separate',
                        help='separate equality constraints into lower and upper constraints.',
                        action='store_true')
    args = parser.parse_args()

    mps_list_file_name = args.mps_list

    with open(mps_list_file_name, 'r') as f:
        mps_list = json.load(f)

    results = run_benchmark(args.executable,
                            mps_list,
                            args.option,
                            args.separate)

    with open('benchmark_result.json', 'w') as f:
        json.dump(results, f, indent=4)

    write_csv(results)

###############################################################################


if __name__ == "__main__":
    main()

###############################################################################
# END
###############################################################################
