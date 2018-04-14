# runtimepad

`runtimepad` is like a wrapper to make sure the repeated evaluations of a function end up (almost) the same total time.

An example use case: The function does pre-computation when the system is idle. If the time of each pre-computation evaluation may be sensitive (have timing pattern), we can reduce this pattern by running it to a specific total time.

*Acknowledgment:*

- Yishuai Li @liyishuai provided some helpful discussion.
- Signal and timer for more accurate wait is proposed by dwhitney67 in https://ubuntuforums.org/showthread.php?t=1372924.
