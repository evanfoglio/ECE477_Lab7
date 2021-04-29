import matplotlib.pyplot as plot
average_data_file = open("cold_average.dat", "r")
data = average_data_file.readlines()
sum_data_file = open("cold_sum.dat", "r")
sum_data = sum_data_file.readlines()
time = []
count = 32


for i in data:
    time.append(count)
    count += 1

plot.plot(time, data)
plot.title("Rolling average reading for 32 seconds close to room temprature 25C")
plot.xlabel("Time, seconds")
plot.ylabel("Average value (mv)")
plot.savefig("average_data_plot.png")
plot.clf()

plot.plot(time, sum_data)
plot.title("Rolling sum reading for 32 seconds close to room temprature 25C")
plot.xlabel("Time, seconds")
plot.ylabel("Sum value (mv)")
plot.savefig("sum_data_plot.png")
