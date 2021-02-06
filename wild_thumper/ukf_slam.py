import numpy as np


def ut_sigma_points(x, P, ut_params):
    """Create sigma points for vector x and P using the unscented transform

    Parameters
    ----------
    x: ndarray
        x vector (mean)
    P: ndarray
        covariance matrix of x
    ut_params: dict
        unscented transformation parameters (floats) {'alpha', 'kappa', 'beta'}

    Returns
    -------
    X: ndarray
        sigma points
    w_m: ndarray
        weights for mean reconstruction
    w_c: ndarray
        weights for covariance matrix reconstruction

    """

    # unscented transform parameters
    n = x.size  # dimensionality
    alpha = ut_params['alpha']
    kappa = ut_params['kappa']
    lmbd = alpha ** 2 * (n + kappa) - n  # scaling parameter lambda
    beta = ut_params['beta']

    # weights for UT reconstruction
    w_m_0 = lmbd / (n + lmbd)  # weight 0 for mean reconstruction
    w_c_0 = lmbd / (n + lmbd) + (1 - alpha ** 2 + beta)  # weight 0 for covariance reconstruction
    w_i = 1 / (2 * (n + lmbd))  # weight i for mean and covariance reconstruction

    w_m = np.array([w_m_0] + [w_i] * 2 * n)
    w_c = np.array([w_c_0] + [w_i] * 2 * n)

    # compute square root of matrix P using the Cholesky decomposition
    P_sq = np.linalg.cholesky((n + lmbd) * P)

    # sigma points
    X_0 = x  # point 0 is mean
    X = X_0

    # other sigma points
    for i in range(0, n):
        X = np.c_[X, x + P_sq[:, i]]

    for i in range(n):
        X = np.c_[X, x - P_sq[:, i]]

    return X, w_m, w_c


def ut_reconstruct(X, w_m, w_c):
    """Reconstruct mean and covariance from sigma points UT)

    Parameters
    ----------
    X: ndarray
        sigma points
    w_m: ndarray
        weights for mean reconstruction
    w_c: ndarray
        weights for covariance matrix reconstruction

    Returns
    -------
    x: ndarray
        x vector (mean)
    P: np.ndarray
        covariance matrix of x
    """

    # reconstruct mean
    x = np.sum(X * w_m, axis=1)

    # reconstruct covariance matrix
    S = np.zeros((X.shape[0], X.shape[0]))
    for i in range(X.shape[1]):
        S_i = w_c[i] * (X[:, i] - x).reshape(-1, 1).dot((X[:, i] - x).reshape(-1, 1).T)
        S += S_i

    return x, S


def ut_cross_covarance(X, Z, w_m, w_c):
    """Calculate cross covariance matrix between two vector x,z, which ares stored in a form of sigma points

    Parameters
    ----------
    X: ndarray
        sigma vectors for x
    Z: ndarray
        sigma vectors for z
    w_m: ndarray
        UT weights for mean reconstruction
    w_c: ndaarray
        UT weights for covariance reconstruction

    Returns
    -------
    P_xz: ndarray
        cross covariance matrix
    """
    # reconstruct means
    x = np.sum(X * w_m, axis=1)
    z = np.sum(Z * w_m, axis=1)

    # print(w_c)
    P_xz = np.zeros((X.shape[0], Z.shape[0]))
    for i in range(2 * x.size + 1):
        Pxzi = w_c[i] * (X[:, i] - x).reshape(-1, 1).dot((Z[:, i] - z).reshape(-1, 1).T)
        #     print(P_i)
        P_xz += Pxzi

    # print('iters   ', 2 * x.size + 1)

    return P_xz
