#!/usr/bin/env python3

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

def Toom_Matrix2 (K,q) :
    # input is the size of the toom
    assert (K == int(K) and K >= 3)
    # output is a 2-dimensional array, the Toom interpolation matrix mod q
    matrix_eval = matrix(QQ, 2*K-1, 2*K-1)
    points_eval = [0 for i in range(2*K-1)]
    points_eval[1] = infinity
    points_eval[2] = 1
    points_eval[3] = -1
    for i in range(1,int((K-1)/2)) :
        points_eval[4*i] = 2^i
        points_eval[4*i+1] = - 2^i
        points_eval[4*i+2] = 2^(-i)
        points_eval[4*i+3] = - 2^(-i)
    if (K % 2 == 0) :
        points_eval[2*K-4] = 2^((K-2)/2)
        points_eval[2*K-3] = - 2^((K-2)/2)
        points_eval[2*K-2] = 2^(-(K-2)/2)
    else :
        points_eval[2*K-2] = 2^((K-1)/2)
    #print points_eval
    matrix_eval[0,0] = 1
    matrix_eval[1,2*K-2] = 1
    for i in range(2,2*K-1) :
        if (points_eval[i] == int(points_eval[i])) :
            for j in range(2*K-1) :
                matrix_eval[i,j] = points_eval[i]^j
        else :
            for j in range(2*K-1) :
                matrix_eval[i,j] = points_eval[i]^(j-2*K+2)
    #return(matrix_eval)
    matrix_interpol = matrix_eval.inverse()
    for i in range(2,2*K-1) :
        if (points_eval[i] != int(points_eval[i])) :
            for j in range(2*K-1) :
                matrix_eval[i,j] = points_eval[i]^(j-K+1)
    matrix_eval[1,K-1] = 1
    for i in range(2*K-1) :
        for j in range(2*K-1) :
            matrix_interpol[i,j] = cmod(int(GF(q)(matrix_interpol[i,j])),q)
    return(str(sage_input(matrix_eval.delete_columns(range(K,2*K-1))))[11:-1],
           str(sage_input(matrix_interpol))[11:-1])

def Toom_Matrix3 (K,q) :
    # input is the size of the toom
    assert (K == int(K) and K >= 3)
    # output is a 2-dimensional array, the Toom interpolation matrix mod q
    matrix_eval = matrix(QQ, 2*K-1, 2*K-1)
    points_eval = [0 for i in range(2*K-1)]
    points_eval[1] = infinity
    points_eval[2] = 1
    points_eval[3] = -1
    for i in range(1,int((K-1)/2)) :
        points_eval[4*i] = 2^i
        points_eval[4*i+1] = - 2^i
        points_eval[4*i+2] = 2^(-i)
        points_eval[4*i+3] = - 2^(-i)
    if (K % 2 == 0) :
        points_eval[2*K-4] = 2^((K-2)/2)
        points_eval[2*K-3] = - 2^((K-2)/2)
        points_eval[2*K-2] = 2^(-(K-2)/2)
    else :
        points_eval[2*K-2] = 2^((K-1)/2)
    #print points_eval
    matrix_eval[0,0] = 1
    matrix_eval[1,2*K-2] = 1
    for i in range(2,2*K-1) :
        if (points_eval[i] == int(points_eval[i])) :
            for j in range(2*K-1) :
                matrix_eval[i,j] = points_eval[i]^j
        else :
            for j in range(2*K-1) :
                matrix_eval[i,j] = points_eval[i]^(j-2*K+2)
    #return(matrix_eval)
    matrix_interpol = matrix_eval.inverse()
    for i in range(2,2*K-1) :
        if (points_eval[i] != int(points_eval[i])) :
            for j in range(2*K-1) :
                matrix_eval[i,j] = points_eval[i]^(j-K+1)
    matrix_eval[1,K-1] = 1
    k = 0
    for i in range(2*K-1) :
        for j in range(2*K-1) :
            fd = factor(denominator(matrix_interpol[i,j]))
            if (2 in [p for p,e in fd]) :
                for p,e in fd :
                    if (p == 2) : k = max(k, e)
    for i in range(2*K-1) :
        for j in range(2*K-1) :
            matrix_interpol[i,j] = cmod(matrix_interpol[i,j]*(2**k),q)
            #print(matrix_interpol[i,j])
    #return((matrix_eval,matrix_interpol,points_eval,2**k))
    return(str(sage_input(matrix_eval.delete_columns(range(K,2*K-1))))[11:-1],
           str(sage_input(matrix_interpol))[11:-1],k)


def Toom_Matrices_build(L) :

    print("Toom_Matrices = {}")
    
    for (K,N) in L :
        M0,M1,l = Toom_Matrix3(K,N)
        print("Toom_Matrices[(%d,%d)]=(%s,%s,%d)" % (K,N,M0,M1,l))

Toom_Matrices_build([(3,8192),(3,2048),(4,8192),(4,2048),(5,2048)])
