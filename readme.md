Summary of Project:
This project is about creating a monitoring tool in the terminal that retrieves system data overtime and with specific delays
about memory usage, cpu data, and core data and representing this data in a graph that gets updated every delay (represents core
data through the use of a diagram)

-------------------------------------------------------------------------------------------------------------------------------------------
How to run the program and Expected Results:

 You need to compile all the files using the "make" command in terminal
 Then to run it, you'll enter "./myProgram" to run the default program

 The command-line commands include:
 [[samples] [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]

 Based on these commands, they dictate which pieces of the code is run, like if you only run --memory, then only the memory
 graph will be displayed, or --cpu will lead to only the cpu graph being displayed or cores will lead to only the cores being 
 displayed, but running any command that has to do with the delay or the number of samples does not affect which graphs or displays 
 are shown (--samples=N or --tdelay=T)

 The commands should come right after your run command
 For example: ./myProgram --memory --cpu --samples=100 --tdelay==100000
 and then you can press enter to run the program to see your desired displays

 During the program if you press CTRL+C, you will get a prompt that asks you whether or not you want to quit the program, and
 you either enter "y" to quit the program or "n" to not quit the program (continue).
 Pressing CTRL+Z will do nothing and the program will keep running and will ignore CTRL+z

----------------------------------------------------------------------------------------------------------------------------------------------
How the program works:

 Memory usage was calculated by going through the /proc/meminfo file, getting the total memory and the memory available values and then
 findng the difference, and then to find the position I just divided the difference by the total memory and then multiplied by 10.

 Cpu usage was calculated by taking the current and the previous srksnapshot of cpu usage using the expression:
 ((total_2 - total_1) - (idle_2 - idle_1)) / (total_2 - total_1)
 Where any values with a subscript of 2 representing values from the current snapshot and any values with a subscript of 1 represents
 values from the previous snapshot.
 To find the values I have functions that parse through /proc/stat to get the info

 Both the cpu graph and memory graph have 10 increments for the y-axis, to represent percentages of usage more accurately,
 and both the cpu graph and memory graph round the given percentage of usage, the cpu graph rounds based off conventional rounding
 where 5 and up, it'll round up, 4 and below it'll round down, the cpu graph always rounds to the nearest tens place, but the rounding 
 only applies to the y-position of the plot, the memory graph always rounds the positioning down to the nearest whole number.

 NOTE: If the positioning of the point on the graph rounds down to 0, the point is not displayed, so any samples that are not plotted
 represent 0 on the graph, but the number representing the usage of either cpu or memory at the header of their respective graph give
 the accurate usage of the component (cpu or RAM)

 The way I went about creating the code to graph the samples is that the values are saved in an array and after delays the information
 is updated and so is the array and the graphs and the entire screen get wiped and then the graphs get re-printed

 NOTE: If you shorten or enlarge the terminal, there is a possibility that the screen won't wipe properly and if you scroll up there will
 be the graphs that weren't wiped properly remaining, problems can also occur if the terminal is too small to fit all the graphs
