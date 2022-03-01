#!/usr/bin/env python3

from math import log,ceil,floor,sqrt


def cmod (a,b) :
    assert (b>0)
    r = (a % b)
    if (r >= b/2) : return (r-b)
    else : return r

def find_zeta (q, n) :
    for i in range(2,q) :
        prod = i
        for j in range(2,n) :
            prod *= i
            prod %= q
            if (prod == 1) : break
        if (j == n-1 and prod * i % q == 1) : return (cmod(i,q))

def extended_gcd(a, b) :
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = extended_gcd(b % a, a)
        return (g, x - (b // a) * y, y)

def mod_inverse(a, m) :
    g, x, y = extended_gcd(a, m)
    if (g ** 2 != 1):
        raise Exception('modular inverse does not exist')
    elif (g == 1):
        return x % m
    else : # (g == -1)
        return (-x) % m
    
def cmod_inverse(a, m) :
    return cmod(mod_inverse(a, m), m)

def transposeMatrix(m):
    return list(map(list,zip(*m)))

def getMatrixMinor(m,i,j):
    return [row[:j] + row[j+1:] for row in (m[:i]+m[i+1:])]

def getMatrixDeterminant(m,q):
    N = len(m)
    A = m
    for i in range(N) : # identify pivot
        j = i
        while (A[j][i] % q == 0) :
            j += 1
            if (j == N) : return(0)
        for k in range(i,N) :
            A[i][k],A[j][k] = A[j][k],A[i][k]
        pivotr = mod_inverse(A[i][i],q)
        for j in range(i+1,N) :
            multiplier = A[j][i] * pivotr
            for k in range(i+1,N) :
                A[j][k] -= A[i][k] * A[j][i]
            A[j][i] = 0
    determinant = 1
    for i in range(N) : determinant *= A[i][i]
    return(cmod(determinant,q))

def getMatrixInverse(m,q) :
    N = len(m)
    A = [ row + ([0] * N) for row in m]
    for i in range(N) : A[i][i+N] = 1
    for i in range(N) : # identify pivot
        j = i
        while (A[j][i] % q == 0) :
            j += 1
            if (j == N) : return(-1)
        for k in range(i,2*N) :
            A[i][k],A[j][k] = A[j][k],A[i][k]
        multiplier = mod_inverse(A[i][i],q)
        A[i][i] = 1
        for k in range(i+1,2*N) :
            A[i][k] *= multiplier
        for j in range(i) :
            for k in range(i+1,2*N) :
                A[j][k] -= A[i][k] * A[j][i]
            A[j][i] = 0
        for j in range(i+1,N) :
            for k in range(i+1,2*N) :
                A[j][k] -= A[i][k] * A[j][i]
            A[j][i] = 0
    A = [ list(map(lambda x : cmod(x,q), row[N:])) for row in A]
    return(A)
    
def getMatrixInverse2(m,q) :
    _K = int(round(log(q)/log(2))) ; assert(q == 2**_K)
    N = len(m)
    A = [ row + ([0] * N) for row in m]
    for i in range(N) : A[i][i+N] = 1
    for i in range(N) : # identify pivot
        L = 1 ; j = N
        while (j == N) :
            L *= 2 ; j = i
            while (A[j][i] % L == 0 and j < N) : j += 1
        for k in range(i,2*N) :
            A[i][k],A[j][k] = A[j][k],A[i][k]
        multiplier = mod_inverse(A[i][i]//(L//2),q)
        A[i][i] = L
        for k in range(i+1,2*N) :
            A[i][k] *= multiplier
        for j in range(i+1,N) :
            for k in range(i+1,2*N) :
                A[j][k] -= A[i][k] * (A[j][i]//L)
            A[j][i] = 0
        for j in range(i) :
            for k in range(i+1,2*N) :
                A[j][k] -= A[i][k] * (A[j][i]//L)
            A[j][i] = 0
    L = max(A[i][i] for i in range(N))
    A = [[cmod(A[i][j+N]*L//A[i][i],q) for j in range(N)] for i in range(N)]
    return(L,A)
    




def Toom_Matrix (K,q=0) :
    # input is the size of the toom and the modulus
    assert (K == int(K) and K >= 3)
    # output is a 2-dimensional array, the Toom interpolation matrix mod q
    points_eval = [0] + list(range(K)) + list(range(-K+2,0))
    # points_eval[1]=inf points_eval[0]=0
    matrix_eval = [[1] + [0] * (2*K-2), [0] * (2*K-2) + [1]]
    for i in range(2,2*K-1) :
        matrix_eval += [[points_eval[i]**j for j in range(2*K-1)]]
    matrix_interpol = getMatrixInverse(matrix_eval,q)

    matrix_eval = list(map(lambda L: L[:K], matrix_eval))
    matrix_eval[1][K-1] = 1
    return((matrix_eval,
            matrix_interpol))

