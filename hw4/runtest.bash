#!/bin/bash
##Usage: runtest.bash ./code experiment`date+%Y%m%d%M`

EXE=$1
CORES=$(grep -c '^processor' /proc/cpuinfo)
CSVFILE=$2_${CORES}cores.csv
for threads in 2 4 8 16
do
	for loops in 20000000 40000000 80000000 160000000
	do
		/usr/bin/time -f "$CORES, $loops, $threads, %e, %U, %S" \
			--append --quiet --output=raw_${CSVFILE} \
			$1 $loops $threads		
		/usr/bin/time -f "$CORES, $loops, $threads, %e, %U, %S" \
			--append --quiet --output=raw_${CSVFILE} \
			$1 $loops $threads
		/usr/bin/time -f "$CORES, $loops, $threads, %e, %U, %S" \
			--append --quiet --output=raw_${CSVFILE} \
			$1 $loops $threads
	done
done

# Need to calculate stddev, Installing sqlite extension
rm extension-functions.c*
wget http://sqlite.org/contrib/download/extension-functions.c/\
download/extension-functions.c?get=25
mv extension-functions.c?get=25 extension-functions.c
rm libsqlitefunctions.so
# https://stackoverflow.com/a/16682644/235820
gcc -fPIC -shared extension-functions.c -o libsqlitefunctions.so -lm
mv libsqlitefunctions.so /usr/local/lib/

sqlite3 -batch ${CSVFILE}.db <<EOF
-- create the table
.load /usr/local/lib/libsqlitefunctions.so
CREATE TABLE experiment (
	cores integer,
	loops integer,
	threads integer,
	real_time real,
	user_time real,
	kernel_time real
);
.separator ","
-- load the data file to the table
.import raw_${CSVFILE} experiment
-- create a new table with the errors
CREATE TABLE experiment_error AS
	SELECT cores, loops, threads,
		avg(real_time) as avg_rt, stdev(real_time) as std_rt,
		avg(user_time) as avg_ut, stdev(user_time) as std_ut,
		avg(kernel_time) as avg_kt, stdev(kernel_time) as std_kt
	FROM experiment
	GROUP BY cores, loops, threads;
EOF

# Write data to new file
sqlite3 -csv ${CSVFILE}.db "SELECT * from experiment_error;" > ${CSVFILE}

# Use python to produce plot of results for $CORES
## x -- loops
## y -- time (seconds)
## color -- thread
## dotted --

python3 - <<EOF
import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import os
import pandas as pd
import sys

cores = '${CORES}'
csv_file = '${CSVFILE}'

# https://pandas.pydata.org/pandas-docs/stable/generated/pandas.read_csv.html
df = pd.read_csv(csv_file, names=['cores',
		'loops',
		'threads',
		'avg_rt',
		'std_rt',
		'avg_ut',
		'std_ut',
		'avg_kt',
		'std_kt'])

# split by threads
df2 = df.query('threads == 2')
df4 = df.query('threads == 4')
df8 = df.query('threads == 8')
df16 = df.query('threads == 16')

plt.figure(1)

# https://pandas.pydata.org/pandas-docs/stable/generated/pandas.DataFrame.plot.html
title = "Semaphore addition time for multiple loops and threads on ${CORES} cores[Real Time]"
df2.plot(x='loops', y='avg_rt', yerr='std_rt',
	title=title, color='red' )
df4.plot(x='loops', y='avg_rt', yerr='std_rt',
	title=title, color='green' )
df8.plot(x='loops', y='avg_rt', yerr='std_rt',
	title=title, color='blue')
df16.plot(x='loops', y='avg_rt', yerr='std_rt',
	title=title, color='yellow' )

# https://matplotlib.org/api/_as_gen/matplotlib.pyplot.fill_between.html
plt.fill_between(df2['loops'],
			df2['avg_rt'] + df2['std_rt'],
			df2['avg_rt'] - df2['std_rt'],
			interpolate=False,
			color='red',
			alpha=0.2)
plt.fill_between(df4['loops'],
			df4['avg_rt'] + df4['std_rt'],
			df4['avg_rt'] - df4['std_rt'],
			interpolate=False,
			color='green',
			alpha=0.2)
plt.fill_between(df8['loops'],
			df8['avg_rt'] + df8['std_rt'],
			df8['avg_rt'] - df8['std_rt'],
			interpolate=False,
			color='blue',
			alpha=0.2)
plt.fill_between(df16['loops'],
			df16['avg_rt'] + df16['std_rt'],
			df16['avg_rt'] - df16['std_rt'],
			interpolate=False,
			color='yellow',
			alpha=0.2)

plt.ylabel('Time (s)')
plt.xlabel('Loops')

plt.legend(['Threads 2', 'Threads 4', 'Threads 8', 'Threads 16'],
		loc='upper left')

# Save the two figures
plt.savefig("{}_rt.png".format(csv_file), bbox_inches='tight')
plt.savefig("{}_rt.pdf".format(csv_file), bbox_inches='tight')

plt.figure(2)

title = "Semaphore addition time for multiple loops and threads on ${CORES} cores[User Time]"
df2.plot(x='loops', y='avg_ut', yerr='std_ut',
	title=title, color='red' )
df4.plot(x='loops', y='avg_ut', yerr='std_ut',
	title=title, color='green' )
df8.plot(x='loops', y='avg_ut', yerr='std_ut',
	title=title, color='blue')
df16.plot(x='loops', y='avg_ut', yerr='std_ut',
	title=title, color='yellow' )

# https://matplotlib.org/api/_as_gen/matplotlib.pyplot.fill_between.html
plt.fill_between(df2['loops'],
			df2['avg_ut'] + df2['std_ut'],
			df2['avg_ut'] - df2['std_ut'],
			interpolate=False,
			color='red',
			alpha=0.2)
plt.fill_between(df4['loops'],
			df4['avg_ut'] + df4['std_ut'],
			df4['avg_ut'] - df4['std_ut'],
			interpolate=False,
			color='green',
			alpha=0.2)
plt.fill_between(df8['loops'],
			df8['avg_ut'] + df8['std_ut'],
			df8['avg_ut'] - df8['std_ut'],
			interpolate=False,
			color='blue',
			alpha=0.2)
plt.fill_between(df16['loops'],
			df16['avg_ut'] + df16['std_ut'],
			df16['avg_ut'] - df16['std_ut'],
			interpolate=False,
			color='yellow',
			alpha=0.2)

plt.ylabel('Time (s)')
plt.xlabel('Loops')

plt.legend(['Threads 2', 'Threads 4', 'Threads 8', 'Threads 16'],
		loc='upper left')

# Save the two figures
plt.savefig("{}_ut.png".format(csv_file), bbox_inches='tight')
plt.savefig("{}_ut.pdf".format(csv_file), bbox_inches='tight')

plt.figure(3)

title = "Semaphore addition time for multiple loops and threads on ${CORES} cores[Kernel Time]"
df2.plot(x='loops', y='avg_kt', yerr='std_kt',
	title=title, color='red' )
df4.plot(x='loops', y='avg_kt', yerr='std_kt',
	title=title, color='green' )
df8.plot(x='loops', y='avg_kt', yerr='std_kt',
	title=title, color='blue')
df16.plot(x='loops', y='avg_kt', yerr='std_kt',
	title=title, color='yellow' )

# https://matplotlib.org/api/_as_gen/matplotlib.pyplot.fill_between.html
plt.fill_between(df2['loops'],
			df2['avg_kt'] + df2['std_kt'],
			df2['avg_kt'] - df2['std_kt'],
			interpolate=False,
			color='red',
			alpha=0.2)
plt.fill_between(df4['loops'],
			df4['avg_kt'] + df4['std_kt'],
			df4['avg_kt'] - df4['std_kt'],
			interpolate=False,
			color='green',
			alpha=0.2)
plt.fill_between(df8['loops'],
			df8['avg_kt'] + df8['std_kt'],
			df8['avg_kt'] - df8['std_kt'],
			interpolate=False,
			color='blue',
			alpha=0.2)
plt.fill_between(df16['loops'],
			df16['avg_kt'] + df16['std_kt'],
			df16['avg_kt'] - df16['std_kt'],
			interpolate=False,
			color='yellow',
			alpha=0.2)

plt.ylabel('Time (s)')
plt.xlabel('Loops')

plt.legend(['Threads 2', 'Threads 4', 'Threads 8', 'Threads 16'],
		loc='upper left')

# Save the two figures
plt.savefig("{}_kt.png".format(csv_file), bbox_inches='tight')
plt.savefig("{}_kt.pdf".format(csv_file), bbox_inches='tight')

EOF
