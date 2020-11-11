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
    results = []
    index = 0
    for mps in mps_list:
        commands = [executable, mps]
        if (is_enabled_separate_equality):
            commands.append('--separate')
        if (option_file_name):
            commands.append('-p')
            commands.append(option_file_name)

        print('#{} Solving'.format(index) + mps + '...')
        subprocess.run(commands)

        with open('status.json', 'r') as f:
            status = json.load(f)
        with open('incumbent.json', 'r') as f:
            incumbent = json.load(f)

        result = {
            'instance': {
                'name': status['name'],
                'number_of_variables': status['number_of_variables'],
                'number_of_constraints': status['number_of_constraints']
            },
            'computed': {
                'is_found_feasible_solution': status['is_found_feasible_solution'],
                'objective': incumbent['objective'],
                'total_violation': incumbent['total_violation'],
                'elapsed_time': status['elapsed_time'],
                'number_of_lagrange_dual_iterations': status['number_of_lagrange_dual_iterations'],
                'number_of_local_search_iterations': status['number_of_local_search_iterations'],
                'number_of_tabu_search_iterations': status['number_of_tabu_search_iterations'],
                'number_of_tabu_search_loops': status['number_of_tabu_search_loops']
            }
        }
        results.append(result)
        print('  n.variables:{0}, n.constraints:{1}, is_feasible:{2}, objective:{3}, total violation:{4}, elapsed time[sec]:{5}'.format(
            status['number_of_variables'],
            status['number_of_constraints'],
            status['is_found_feasible_solution'],
            incumbent['objective'],
            incumbent['total_violation'],
            status['elapsed_time']
        ))
        print()
        index += 1

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
            f.write('{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10}\n'.format(
                result['instance']['name'],
                result['instance']['number_of_variables'],
                result['instance']['number_of_constraints'],
                result['computed']['is_found_feasible_solution'],
                result['computed']['objective'],
                result['computed']['total_violation'],
                result['computed']['elapsed_time'],
                result['computed']['number_of_lagrange_dual_iterations'],
                result['computed']['number_of_local_search_iterations'],
                result['computed']['number_of_tabu_search_iterations'],
                result['computed']['number_of_tabu_search_loops']
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
