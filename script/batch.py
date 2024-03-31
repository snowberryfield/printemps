# -*- coding: utf-8 -*-
###############################################################################
# Copyright (c) 2020-2024 Yuji KOGUMA
# Released under the MIT license
# https://opensource.org/licenses/mit-license.php
###############################################################################

import subprocess
import json
import argparse
import numpy as np

###############################################################################


def run_batch(executable, mps_list, option_file_name):
    results = []
    index = 0
    print('-----+-------------+----------+-------------+-------------+-----------+--------')
    print(
        ' no. | name        | feasible |   objective |  known best | violation | time[s]')
    print('-----+-------------+----------+-------------+-------------+-----------+--------')

    for mps in mps_list:
        commands = [executable, mps]
        if (option_file_name):
            commands.append('-p')
            commands.append(option_file_name)
        subprocess.run(commands)

        with open('status.json', 'r') as f:
            status = json.load(f)
            f.close()

        with open('incumbent.json', 'r') as f:
            incumbent = json.load(f)
            f.close()

        result = {
            'instance': {
                'name':
                    status['name'],
                'number_of_variables':
                    status['number_of_variables'],
                'number_of_constraints':
                    status['number_of_constraints']
            },
            'computed': {
                'is_found_feasible_solution':
                    status['is_found_feasible_solution'],
                'objective':
                    incumbent['objective'],
                'total_violation':
                    incumbent['total_violation'],
                'elapsed_time':
                    status['elapsed_time'],
                'number_of_lagrange_dual_iterations':
                    status['number_of_lagrange_dual_iterations'],
                'number_of_local_search_iterations':
                    status['number_of_local_search_iterations'],
                'number_of_tabu_search_iterations':
                    status['number_of_tabu_search_iterations'],
                'number_of_tabu_search_loops':
                    status['number_of_tabu_search_loops']
            }
        }
        results.append(result)
        print(' {0:03d} | {1:11s} | {2:8s} | {3:11.4e} | {4:11.4e} | {5:9.3e} | {6:7.1f}'.format(
            index,
            result['instance']['name'][:11],
            'Yes' if result['computed']['is_found_feasible_solution'] == 1 else 'No',
            np.nan if result['computed']['objective'] == 'N/A' else result['computed']['objective'],
            np.nan,
            np.nan if result['computed']['total_violation'] == 'N/A' else result['computed']['total_violation'],
            np.nan if result['computed']['elapsed_time'] == 'N/A' else result['computed']['elapsed_time']
        ))
        index += 1

    return results

###############################################################################


def write_csv(results):
    with open('batch_result.csv', 'w') as f:

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
        description='A script for batch execution of printemps')
    parser.add_argument('executable',
                        help='specify the executable of printemps.',
                        type=str)
    parser.add_argument('mps_list',
                        help='specify the mps list file name.',
                        type=str)
    parser.add_argument('-p', '--option',
                        help='specify the option file name.',
                        type=str)
    args = parser.parse_args()

    mps_list_file_name = args.mps_list

    with open(mps_list_file_name, 'r') as f:
        mps_list = json.load(f)

    results = run_batch(args.executable,
                        mps_list,
                        args.option)

    with open('batch_result.json', 'w') as f:
        json.dump(results, f, indent=4)

    write_csv(results)

###############################################################################


if __name__ == "__main__":
    main()

###############################################################################
# END
###############################################################################
