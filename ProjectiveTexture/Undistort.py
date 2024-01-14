import numpy as np
import cv2
import glob
import argparse

############### Configure ################

# Image to be dewarped (different image for different FOV)
image_path = './resources/Vehicle1_Way0/180/0/1313.png'

# Intrinsic and distortion parameters for each camera (from calibration)
# 180
DIM   = (1280, 1280)
K     = np.array([[696.0405939705771, 0.0, 640.0],
				  [0.0, 694.8901322581238, 640.0],
				  [0.0, 0.0, 1.0]])
xi    = np.array([[0.7100823796212375]])
D     = np.array([[-0.2894618100329196, 0.06539145347156397, 0.0002772246603313237, 9.335154355321922e-05]])

# 160
# DIM   = (1280, 1280)
# K     = np.array([[732.2980640352715, 0.0, 640.0],
# 				  [0.0, 734.1208095538103, 640.0],
# 				  [0.0, 0.0, 1.0]])
# xi    = np.array([[0.5852392164668814]])
# D     = np.array([[-0.30279907728116356, 0.07291584096650669, 0.0008114609772521696, 3.0358130968293173e-05]])

# 140
# K     = np.array([[849.7623058705126, 0.0, 640.0],
# 				  [0.0, 854.9941117952679, 640.0],
# 				  [0.0, 0.0, 1.0]])
# xi    = np.array([[0.6177725867483063]])
# D     = np.array([[-0.31626897345026533, 0.0908935213159033, -0.0042641514635591606, 0.00047781770666383195]])

# 120
# DIM   = (1280, 1280)
# K     = np.array([[990.5025595428473, 0.0, 640.0],
# 				  [0.0, 989.4925333788054, 640.0],
# 				  [0.0, 0.0, 1.0]])
# xi    = np.array([[0.663287291946311]])
# D     = np.array([[-0.31902080365150165, 0.09533096590378456, -0.0013460992685962586, 0.00036166477357630673]])

##########################################

# Set rescale and vehicle size
parser = argparse.ArgumentParser()
parser.add_argument('-r', default = 12, type = float, help = 'rescale focus lenght')
parser.add_argument('-s', default = 532, type = int, help = 'truck size')
args = parser.parse_args()

rescale     = 1.0/args.r
truck_size  = args.s

# Projection matrix
P     = np.array([[DIM[1]*rescale, 0.0, DIM[1]/2.0],
				  [0.0, DIM[0]*rescale, DIM[0]/2.0], 
				  [0.0, 0.0, 1.0]])
P_inv = np.linalg.inv(P)

# Inversed Undistort (Refer to OpenCV documentation)
def Undistort(i, j):
	_X = P_inv @ np.array([i, j, 1.0])

	_x = _X[0]
	_y = _X[1]
	_w = _X[2]

	r = np.sqrt(_x*_x + _y*_y + _w)
	Xs = _x / r
	Ys = _y / r
	Zs = _w / r

	xu = Xs / (Zs + xi[0,0])
	yu = Ys / (Zs + xi[0,0])

	r2 = xu*xu + yu*yu
	r4 = r2*r2

	xd = (1+D[0,0]*r2+D[0,1]*r4)*xu + 2*D[0,2]*xu*yu + D[0,3]*(r2+2*xu*xu)
	yd = (1+D[0,0]*r2+D[0,1]*r4)*yu + D[0,2]*(r2+2*yu*yu) + 2*D[0,3]*xu*yu

	u = K[0,0]*xd + K[0,2]
	v = K[1,1]*yd + K[1,2]

	return u, v

img = cv2.imread(image_path)
(h,w) = img.shape[:2]

# Undistort without opencv
# out_img = np.zeros((h,w,3), dtype=np.uint8)
# for i in range(h):
# 	for j in range(w):
# 		u,v = Undistort(i,j)
# 		u = int(u)
# 		v = int(v)
# 		if(u<h and u>=0 and v<w and v>=0):
# 			out_img[i,j] = img[u,v]

# Undistort with opencv
map1, map2 = cv2.omnidir.initUndistortRectifyMap(K, D, xi, np.eye(3), P, DIM, cv2.CV_16SC2, cv2.omnidir.RECTIFY_PERSPECTIVE)
out_img = cv2.remap(img, map1, map2, cv2.INTER_LINEAR)


# Draw red line
cv2.line(out_img, (0,DIM[0]-truck_size), (DIM[1],DIM[0]-truck_size), (0,0,255), 4)
cv2.imwrite('Undistort.jpg', out_img)
cv2.imshow('Undistort', out_img)
cv2.waitKey(0)
cv2.destroyAllWindows()
