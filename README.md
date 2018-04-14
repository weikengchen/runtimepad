# runtimepad

`runtimepad` is like a wrapper to make sure the repeated evaluations of a function end up (almost) the same total time.

## Example use case

In an application, we want to do pre-computation when the system is idle. If the time of each pre-computation evaluation is sensitive (have timing pattern), we can reduce this pattern by padding it to a specific total time.

For example, to boost the online efficiency for ElGamal, you can pre-compute the pair (pk^r, g^r) for randomly sampled r (https://github.com/weikengchen/mod_exp_with_precomputation). However, different r results in different evalution times. It is hard to claim the security with very strong confidence. So we use this wrapper to pad the running time.

Note that in this specific use case, `mpz_powm_sec` is another option. But not as efficient, as (1) it pads the time for each single exponentiation, not for the total; (2) it does not reuse the lookup table when we do exponentiation many times for the same base. 

## Acknowledgment

- Yishuai Li @liyishuai provided some helpful discussion.
- The use of signals and timers for accurate wait were proposed by dwhitney67 in https://ubuntuforums.org/showthread.php?t=1372924.
