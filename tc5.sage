TC5X = matrix([[1,0,0,0,0,0,0,0,0],
               [1,1,1,1,1,1,1,1,1],
               [1,-1,1,-1,1,-1,1,-1,1],
               [1,2,4,8,16,32,64,128,256],
               [1,-2,4,-8,16,-32,64,-128,256],
               [1,3,9,27,81,243,729,2187,6561],
               [256,128,64,32,16,8,4,2,1],
               [256,-128,64,-32,16,-8,4,-2,1],
               [0,0,0,0,0,0,0,0,1]])

TC5 = matrix([[1,0,0,0,0],
              [1,1,1,1,1],
              [1,-1,1,-1,1],
              [1,2,4,8,16],
              [1,-2,4,-8,16],
              [1,3,9,27,81],
              [16,8,4,2,1],
              [16,-8,4,-2,1],
              [0,0,0,0,1]])

var('z0','z1','z2','z3','z4','z5','z6','z7','z8')
var('b0','b1','b2','b3','b4')

Z = vector([z0,z1,z2,z3,z4,z5,z6,z7,z8])
B = vector([b0,b1,b2,b3,b4])

TC5I = TC5X.inverse().transpose()
TC5T = TC5.transpose()[::-1,:]

def toeplitz (L) :
    if (len(L) % 2 == 1) : L.insert(0,0)
    n = len(L)/2
    return([L[i+1:n+i+1][::-1] for i in range(n)])

Scale = diagonal_matrix([8,72,72,360,360,4200,360,360,8])
