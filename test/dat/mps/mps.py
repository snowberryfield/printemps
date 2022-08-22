import pulp

problem = pulp.LpProblem('problem', sense=pulp.LpMinimize)
x_range = range(10)
y_range = range(20)
z_range = range(30)
x = pulp.LpVariable.dicts(
    'x', indexs=x_range, lowBound=0, upBound=1, cat=pulp.LpBinary)

y = pulp.LpVariable.dicts(
    'y', indexs=y_range, lowBound=None, upBound=None, cat=pulp.LpContinuous)

z = pulp.LpVariable.dicts(
    'z', indexs=z_range, lowBound=-100, upBound=100, cat=pulp.LpInteger)

problem += pulp.lpSum([x[i] for i in x_range]) == 1
problem += pulp.lpSum([(i+1) * y[i] for i in y_range]) <= 2E7
problem += pulp.lpSum([(i+1) * z[i] for i in z_range]) >= 300

problem += pulp.lpSum([(i+1) * x[i] for i in x_range])
problem.writeMPS('test.mps')
