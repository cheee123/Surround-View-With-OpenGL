import pyrr
import json
import numpy as np
import argparse

######################################
########### Configuration ############
######################################
parser = argparse.ArgumentParser()
parser.add_argument('--min_R', default = 0.5, type = float, help = 'minimum Radius, bottom of the bowl')
parser.add_argument('--max_R', default = 0.5, type = float, help = 'maximum Radius, top of the bowl')
parser.add_argument('--height',default = 0.26, type = float, help = 'height of the bowl')
parser.add_argument('--stackCount', default = 14, type = int, help = 'number of stack')
parser.add_argument('--json', default = './resources/Vehicle5_Way0/160/camera_config_160.json', type = str, help = 'json file path')
parser.add_argument('--json_p', default = None, type = str, help = 'json predict file path')
parser.add_argument('--config', default = 'camera_config1', type = str, help = 'which camera_config')
parser.add_argument('--sixcam', default = 0, type = int, help = '6 cam model')
parser.add_argument('--FOV', default = 180, type = int, help = 'FOV')
# parser.add_argument('--vehicle_size', default = , type = int, help = 'vehicle_size')
# parser.add_argument('--rescale', default = , type = float, help = 'FOV')
args = parser.parse_args()

min_R      = args.min_R
max_R      = args.max_R
height     = args.height
stackCount = args.stackCount
json_file  = args.json
json_file_p= args.json_p
config     = args.config
six_cam    = args.sixcam
FOV        = args.FOV
drawline   = False

# Camera position (for precise vehicle edges)
if(six_cam):
	top_id    = 0
	right_id  = 1
	bottom_id = 3
	left_id   = 5
else:
	top_id    = 0
	right_id  = 1
	bottom_id = 4
	left_id   = 7

# Manually stretch the model to avoid parallax in a specific distance
modelMat = np.array([
					np.array([22.500000, 0.000000, 0.000000, 0.000000]),
					np.array([0.000000, 30.990000, 0.000000, 0.000000]),
					np.array([0.000000, 0.000000, 63.402000, 0.000000]),
					np.array([0.000000, 0.000000, 0.000000, 1.000000])
					])

DIM   = (1280, 1280)

# Set vehicle_size, rescale, and camera calibration parameters based on the FOV
if FOV == 180:
    vehicle_size = 532 / DIM[0]
    rescale = 1 / 12.0
    K = np.array([[696.0405939705771, 0.0, 640.0],
                  [0.0, 694.8901322581238, 640.0],
                  [0.0, 0.0, 1.0]])
    xi = np.array([[0.7100823796212375]])
    D = np.array([[-0.2894618100329196, 0.06539145347156397, 0.0002772246603313237, 9.335154355321922e-05]])
elif FOV == 160:
    vehicle_size = 440 / DIM[0]
    rescale = 1 / 6.0
    K = np.array([[732.2980640352715, 0.0, 640.0],
                  [0.0, 734.1208095538103, 640.0],
                  [0.0, 0.0, 1.0]])
    xi = np.array([[0.5852392164668814]])
    D = np.array([[-0.30279907728116356, 0.07291584096650669, 0.0008114609772521696, 3.0358130968293173e-05]])
elif FOV == 140:
    vehicle_size = 280 / DIM[0]
    rescale = 1 / 3.5
    K = np.array([[849.7623058705126, 0.0, 640.0],
                  [0.0, 854.9941117952679, 640.0],
                  [0.0, 0.0, 1.0]])
    xi = np.array([[0.6177725867483063]])
    D = np.array([[-0.31626897345026533, 0.0908935213159033, -0.0042641514635591606, 0.00047781770666383195]])
elif FOV == 120:
    vehicle_size = 113 / DIM[0]
    rescale = 1 / 2.5
    K = np.array([[990.5025595428473, 0.0, 640.0],
                  [0.0, 989.4925333788054, 640.0],
                  [0.0, 0.0, 1.0]])
    xi = np.array([[0.663287291946311]])
    D = np.array([[-0.31902080365150165, 0.09533096590378456, -0.0013460992685962586, 0.00036166477357630673]])
else:
    raise ValueError("Unsupported FOV value")

P     = np.array([[DIM[1]*rescale, 0.0, DIM[1]/2.0],
				  [0.0, DIM[0]*rescale, DIM[0]/2.0], 
				  [0.0, 0.0, 1.0]])
P_inv = np.linalg.inv(P)



# Auto calculate new FOV
fov = 2 * np.arctan(0.5 * DIM[1] / P[0,0]) * (180 / np.pi)
# fov = 160.93
print('New fov:', fov)

######################################
######################################
######################################


# Reversed Undistort (Refer to OpenCV documentation)
def Undistort(i, j):
	i *= DIM[1]
	j *= DIM[0]

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

	u /= DIM[1]
	v /= DIM[0]

	return u, v

def UnrealToOpenGLAxis(x, y, z):
	_x = x
	_y = y
	_z = z
	x  = _y
	y  = _z
	z  = -_x
	return [x, y, z]
	
def UnrealToOpenGLRotation(x, y, z):
	_x = x
	_y = y
	_z = z
	x  = -_y
	y  = _z
	z  = _x
	return [x, y, z]

# Find proj and view matrices
if(json_file_p):
	f               = open(json_file_p)
else:
	f               = open(json_file)

data            = json.load(f)
cameraList      = data["camera_list"]
numCam          = len(data[config])
chosen_cam      = []
camera_position = []
projTBO         = []
viewTBO         = []
aspect = 1.0
near   = 0.1
far    = 1000.0

for i, cam in enumerate(data[config]):
	chosen_cam.append(data[config]['camera{}'.format(i+1)])

top_id    = chosen_cam.index(top_id)
right_id  = chosen_cam.index(right_id)
bottom_id = chosen_cam.index(bottom_id)
left_id   = chosen_cam.index(left_id)

# print('Camera position check: (should be clockwise)')
for cc in chosen_cam:
	cameraInfo = cameraList[cc]
	position   = cameraInfo["position"]
	rotation   = cameraInfo["rotation"]
	position = np.array(UnrealToOpenGLAxis(position[0], position[1], position[2]))
	# print(position)
	rotation = UnrealToOpenGLRotation(rotation[0], rotation[1], rotation[2])
	rotation = np.array(rotation) * (np.pi / 180.0)

	rotation_matrix_x = pyrr.matrix33.create_from_x_rotation(rotation[0])
	rotation_matrix_y = pyrr.matrix33.create_from_y_rotation(rotation[1])
	rotation_matrix_z = pyrr.matrix33.create_from_z_rotation(rotation[2])

	combined_rotation_matrix = pyrr.matrix33.multiply(rotation_matrix_z, rotation_matrix_y)
	combined_rotation_matrix = pyrr.matrix33.multiply(combined_rotation_matrix, rotation_matrix_x)

	combined_rotation_matrix[0,2] = -combined_rotation_matrix[0,2]
	combined_rotation_matrix[1,2] = -combined_rotation_matrix[1,2]
	combined_rotation_matrix[2,0] = -combined_rotation_matrix[2,0]
	combined_rotation_matrix[2,1] = -combined_rotation_matrix[2,1]

	forward = np.array([0, 0, -1])
	forward = (combined_rotation_matrix) @ forward
	upDir   = [0, 1, 0]

	projection_matrix = pyrr.matrix44.create_perspective_projection(fov, aspect, near, far)
	projTBO.append(projection_matrix)

	view_matrix = pyrr.matrix44.create_look_at(position, position + forward, upDir)
	viewTBO.append(view_matrix)

	camera_position.append(position)

camera_position = np.array(camera_position)
f.close()

# Transform to Python format
for i in range(len(projTBO)):
	projTBO[i] = projTBO[i].T
for i in range(len(viewTBO)):
	viewTBO[i] = viewTBO[i].T
modelMat = modelMat.T

# Setup parameters for Bowl model
PI          = np.pi
sectorCount = int((min_R+max_R) * PI / height * stackCount)
stackStep   = height / stackCount
sectorStep  = 2.0 * PI / sectorCount
vertices    = []

# Generate vertices for the bowl
for i in range(stackCount+1):
	y      = i * stackStep
	radius = i / stackCount * (max_R-min_R) + min_R
	for j in range(sectorCount):
		sectorAngle = j * sectorStep  # starting from 0 to 2pi

		x = radius * np.sin(sectorAngle)
		z = radius * np.cos(sectorAngle)

		vertices.append(x)
		vertices.append(y)
		vertices.append(z)

# Generate indices
indices = []
for i in range(stackCount):
	for j in range(sectorCount):
		k1 = i  * sectorCount + j      # beginning of current stack
		k2 = (i + 1) * sectorCount + j # beginning of next stack
		if(j == sectorCount-1):
			k3 = i  * sectorCount
			k4 = (i + 1) * sectorCount
		else:
			k3 = k1 + 1 
			k4 = k2 + 1
			
		indices.append(k1)
		indices.append(k2)
		indices.append(k3)
		indices.append(k3)
		indices.append(k2)
		indices.append(k4)
				
bias = len(vertices) // 3

# Generate bottom of the bowl (square version)
min_R_bias = min_R * 0.03   # For better visual result inside the bowl
min_R     *= 1.1
top        = camera_position[top_id][2] / modelMat[2,2] - min_R_bias
right      = camera_position[right_id][0] / modelMat[1,1] + min_R_bias
bottom     = camera_position[bottom_id][2] / modelMat[2,2] + min_R_bias
left       = camera_position[left_id][0] / modelMat[1,1] - min_R_bias

tl_x       = -min_R
tl_z       = -min_R
br_x       = min_R
br_z       = min_R

for ii in range(4):
	if(ii==0):
		rec_w  = right - tl_x
		rec_h  = top - tl_z
		bias_x = tl_x
		bias_z = tl_z
	elif(ii==1):
		rec_w  = br_x - right
		rec_h  = bottom - tl_z
		bias_x = right
		bias_z = tl_z
	elif(ii==2):
		rec_w  = br_x - left
		rec_h  = br_z - bottom
		bias_x = left
		bias_z = bottom
	else:
		rec_w  = left - tl_x
		rec_h  = br_z - top
		bias_x = tl_x
		bias_z = top

	count_w = int(stackCount * rec_w / height)
	count_h = int(stackCount * rec_h / height)
	temp_candi = []
	temp_i     = 0
	for i in range(count_h+1):
		for j in range(count_w+1):
			x = j * rec_w / count_w + bias_x
			z = i * rec_h / count_h + bias_z
			if((x*x + z*z) <= (min_R*min_R)):
				vertices.append(x)
				vertices.append(0.0)
				vertices.append(z)
				temp_candi.append(temp_i)
			temp_i += 1

	# Generate indices
	for i in range(1,count_h+1):
		for j in range(count_w):
			k1 = i  * (count_w+1) + j
			k2 = (i - 1) * (count_w+1) + j
			k3 = k1 + 1
			k4 = k2 + 1

			a_in = k1 in temp_candi
			b_in = k2 in temp_candi
			c_in = k3 in temp_candi
			d_in = k4 in temp_candi

			if a_in and b_in and c_in:
				indices.append(temp_candi.index(k1) + bias)
				indices.append(temp_candi.index(k2) + bias)
				indices.append(temp_candi.index(k3) + bias)
				
			if c_in and b_in and d_in:
				indices.append(temp_candi.index(k3) + bias)
				indices.append(temp_candi.index(k2) + bias)
				indices.append(temp_candi.index(k4) + bias)
				
	bias = len(vertices) // 3

# # Generate bottom of the bowl (circle version)
# stackCount = int(sectorCount / (2 * PI))
# stackStep = min_R / stackCount
# for i in range(sectorCount):
# 	sectorAngle = i * sectorStep
# 	for j in range(stackCount+1):
# 		x = j * stackStep * np.sin(sectorAngle)
# 		z = j * stackStep * np.cos(sectorAngle)
# 		vertices.append(x)
# 		vertices.append(0.0)
# 		vertices.append(z)
# 		# vertices.append(i) # same sectorCount
		
# # Generate indices
# for i in range(sectorCount):
# 	for j in range(stackCount):
# 		k1 = i  * (stackCount+1) + j
# 		if(i == sectorCount-1):
# 			k2 = j
# 		else:
# 			k2 = (i + 1) * (stackCount+1) + j
# 		k3 = k1 + 1
# 		k4 = k2 + 1
		
# 		k1 += bias
# 		k2 += bias
# 		k3 += bias
# 		k4 += bias

# 		indices.append(k1)
# 		indices.append(k2)
# 		indices.append(k3)
# 		indices.append(k3)
# 		indices.append(k2)
# 		indices.append(k4)

vertices_num_ori = len(vertices) // 3
indices_num_ori  = len(indices) // 3
indices_tri = np.reshape(indices, (indices_num_ori,3))

# One-file version
with open('./resources/Bowl_vertices.txt', 'w') as f:
	pass
with open('./resources/Bowl_indices.txt', 'w') as f:
	pass

# Calculate effective vertices
effective     = np.zeros((numCam, vertices_num_ori), dtype=np.float32)
effective_old = np.zeros((numCam, vertices_num_ori), dtype=np.float32)
recommender   = np.zeros((numCam, vertices_num_ori), dtype=np.uint32)

# Calculate the distance between vertices and cameras
distance = np.ones((numCam, vertices_num_ori), dtype=np.float32)
distance *= np.inf
for camera_id in range(numCam):
	for i_ori in range(vertices_num_ori):
		x     = vertices[i_ori*3+0]
		y     = vertices[i_ori*3+1]
		z     = vertices[i_ori*3+2]
		world = modelMat @ np.array([x,y,z,1.0])
		uv    = projTBO[camera_id] @ viewTBO[camera_id] @ world
		uv   /= uv[3]
		uvx   = (uv[0] + 1.0) / 2.0
		uvy   = (uv[1] + 1.0) / 2.0
		if ((uvx < 0.0) or (uvx > 1.0) or
			(uvy < vehicle_size) or (uvy > 1.0) or
			(uv[2] < 0.0) or (uv[2] > 1.0)):
			pass
		else:
			effective_old[camera_id, i_ori] = 1.0
			distance[camera_id, i_ori] = np.sqrt(pow(world[0] - camera_position[camera_id, 0], 2) + pow(world[1] - camera_position[camera_id, 1], 2) + pow(world[2] - camera_position[camera_id, 2], 2))
effective = np.copy(effective_old)

# Update effective based on distance (Camera Weight)
for camera_id in range(numCam):
	for i_ori in range(vertices_num_ori):
		if(effective[camera_id, i_ori]==1.0):
			if np.argmin(np.array(distance[:,i_ori])) != camera_id:
				effective[camera_id, i_ori] = 0.0

# Sort adjacent
adjacent = []
for i in range(vertices_num_ori):
	adjacent.append(set())
for i,tri in enumerate(indices_tri):
	adjacent[tri[0]].add(tri[1])
	adjacent[tri[0]].add(tri[2])
	adjacent[tri[1]].add(tri[0])
	adjacent[tri[1]].add(tri[2])
	adjacent[tri[2]].add(tri[0])
	adjacent[tri[2]].add(tri[1])

# 0 : not effective, 1.0 : effective, 2.0 : dilated, 3.0 : dilated twice
# Dilation 1 
for camera_id in range(numCam):
	for i_ori in range(vertices_num_ori):
		if(effective[camera_id, i_ori]==0.0 and effective_old[camera_id, i_ori]==1.0):
			nearest = []
			for avert in adjacent[i_ori]:
				if(effective[camera_id, avert]==1.0):
					effective[camera_id, i_ori] = 2.0
					nearest.append(avert)
			if(len(nearest)==0):
				pass
			elif(len(nearest)==1):
				recommender[camera_id, i_ori] = nearest[0]
			else:
				dist = []
				for i in nearest:
					dist.append(pow(vertices[i_ori*3+0] - vertices[i*3+0], 2) + pow(vertices[i_ori*3+1] - vertices[i*3+1], 2) + pow(vertices[i_ori*3+2] - vertices[i*3+2], 2))
				recommender[camera_id, i_ori] = nearest[np.argmin(dist)]

# Dilation 2
for camera_id in range(1,numCam):
	for i_ori in range(vertices_num_ori):
		if(effective[camera_id, i_ori]==0.0 and effective_old[camera_id, i_ori]==1.0):
			nearest = []
			for avert in adjacent[i_ori]:
				if(effective[camera_id, avert]==2.0):
					effective[camera_id, i_ori] = 3.0
					nearest.append(avert)
			if(len(nearest)==0):
				pass
			elif(len(nearest)==1):
				recommender[camera_id, i_ori] = nearest[0]
			else:
				dist = []
				for i in nearest:
					dist.append(np.sqrt(pow(vertices[i_ori*3+0] - vertices[i*3+0], 2) + pow(vertices[i_ori*3+1] - vertices[i*3+1], 2) + pow(vertices[i_ori*3+1] - vertices[i*3+1], 2)))
				recommender[camera_id, i_ori] = nearest[np.argmin(dist)]

# Find lonely vertices
lonely = []
for camera_id in range(numCam):
	candidate = set()
	for i_ori in range(vertices_num_ori):
		if(effective[camera_id,i_ori]>0.0):
			candidate.add(i_ori)
	for c in candidate:
		valid1 = False
		valid2 = False
		for a in adjacent[c]:
			if(not valid1):
				if(a in candidate):
					valid1 = True
			else:
				if(a in candidate):
					valid2 = True
					break
		if(not valid2):
			lonely.append(c)
			effective[camera_id,c] = 0.0

# Generate vertices and indices
bias = 0
used = set()
for camera_id in range(numCam):
	candidate = []
	with open('./resources/Bowl_vertices.txt', 'a') as f:
		for i_ori in range(vertices_num_ori):
			if(effective[camera_id, i_ori]>0.0):
				x     = vertices[i_ori*3+0]
				y     = vertices[i_ori*3+1]
				z     = vertices[i_ori*3+2]
				world = modelMat @ np.array([x,y,z,1.0])
				uv    = projTBO[camera_id] @ viewTBO[camera_id] @ world
				uv   /= uv[3]
				uvx   = (uv[0] + 1.0) / 2.0
				uvy   = (uv[1] + 1.0) / 2.0

				f.write(str(world[0])+' '+str(world[1])+' '+str(world[2])+' ')
				uvx, uvy = Undistort(uvx, uvy)
				f.write(str(uvx)+' '+str(uvy))
				f.write(' '+str(camera_id))

				if(camera_id==0):
					f.write(' 1.0\n')
				elif(camera_id==numCam-1):
					if(effective[camera_id, i_ori]==1.0):
						if(2.0 in effective[:, i_ori]):
							dilated_i = list(effective[:, i_ori]).index(2.0)
							if(dilated_i<camera_id and (i_ori in recommender[dilated_i]) and (not i_ori in recommender[camera_id])):
								failed = False
								for avert in adjacent[i_ori]:
									if(not avert in used):
										f.write(' 1.0\n')
										failed = True
										break
								if(failed):
									pass
								else:
									f.write(' 0.5\n')
							else:
								f.write(' 1.0\n')
						else:
							f.write(' 1.0\n')
					elif(effective[camera_id, i_ori]==3.0):
						i_pre = recommender[camera_id, i_ori]
						if(1.0 in effective[:,i_pre]):
							dilated_i = list(effective[:,i_pre]).index(1.0)
							if(i_pre in recommender[dilated_i,:]):
								f.write(' 1.0\n')
							else:
								if(dilated_i < camera_id):
									failed = False
									for avert in adjacent[i_ori]:
										if(not avert in used):
											f.write(' 1.0\n')
											failed = True
											break
									if(failed):
										pass
									else:
										f.write(' 0.5\n')
								else:
									f.write(' 1.0\n')
						else:
							f.write(' 1.0\n')
					else:
						if(1.0 in effective[:, i_ori]):
							des1 = list(effective[:,i_ori]).index(1.0)
							if(2.0 in effective[:,recommender[camera_id, i_ori]]):
								des = list(effective[:,recommender[camera_id, i_ori]]).index(2.0)
								if(des==des1):
									failed = False
									for avert in adjacent[i_ori]:
										if(not avert in used):
											f.write(' 1.0\n')
											failed = True
											break
									if(failed):
										pass
									else:
										f.write(' 0.5\n')
								else:
									f.write(' 1.0\n')
							else:
								f.write(' 1.0\n')
						else:
							f.write(' 1.0\n')
				else:
					if(effective[camera_id, i_ori]==1.0):
						if(2.0 in effective[:, i_ori]):
							dilated_i = list(effective[:, i_ori]).index(2.0)
							if(dilated_i<camera_id and (i_ori in recommender[dilated_i]) and (not i_ori in recommender[camera_id])):
								failed = False
								for avert in adjacent[i_ori]:
									if(not avert in used):
										f.write(' 1.0\n')
										failed = True
										break
								if(failed):
									pass
								else:
									f.write(' 0.5\n')
							else:
								f.write(' 1.0\n')
						else:
							f.write(' 1.0\n')
					elif(effective[camera_id, i_ori]==3.0):
						i_pre = recommender[camera_id, i_ori]
						if(1.0 in effective[:,i_pre]):
							dilated_i = list(effective[:,i_pre]).index(1.0)
							if(i_pre in recommender[dilated_i,:]):
								f.write(' 1.0\n')
							else:
								if(dilated_i < camera_id):
									failed = False
									for avert in adjacent[i_ori]:
										if(not avert in used):
											f.write(' 1.0\n')
											failed = True
											break
									if(failed):
										pass
									else:
										f.write(' 0.5\n')
								else:
									f.write(' 1.0\n')
						else:
							f.write(' 1.0\n')
					else:
						if(1.0 in effective[:, i_ori]):
							des1 = list(effective[:, i_ori]).index(1.0)
							if(des1<camera_id):
								if(2.0 in effective[:,recommender[camera_id, i_ori]]):
									des = list(effective[:,recommender[camera_id, i_ori]]).index(2.0)
									if(des==des1):
										failed = False
										for avert in adjacent[i_ori]:
											if(not avert in used):
												f.write(' 1.0\n')
												failed = True
												break
										if(failed):
											pass
										else:
											f.write(' 0.5\n')
									else:
										f.write(' 1.0\n')
								else:
									f.write(' 1.0\n')
							else:
								f.write(' 1.0\n')
						else:
							f.write(' 1.0\n')
				candidate.append(i_ori)

	# print('Vertices {}:'.format(camera_id), len(candidate))
	count = 0
	if(drawline):
		with open('./resources/Bowl_indices.txt','a') as f:
			for ind in range(indices_num_ori):
				a_in = indices[ind*3+0] in candidate
				b_in = indices[ind*3+1] in candidate
				c_in = indices[ind*3+2] in candidate
				if a_in and b_in and c_in:
					a_index = candidate.index(indices[ind*3+0]) + bias
					b_index = candidate.index(indices[ind*3+1]) + bias
					c_index = candidate.index(indices[ind*3+2]) + bias
					f.write(str(a_index)+' '+str(b_index)+'\n')
					f.write(str(b_index)+' '+str(c_index)+'\n')
					f.write(str(c_index)+' '+str(a_index)+'\n')
					count += 1
	else:
		with open('./resources/Bowl_indices.txt','a') as f:
			for ind in range(indices_num_ori):
				a_in = indices[ind*3+0] in candidate
				b_in = indices[ind*3+1] in candidate
				c_in = indices[ind*3+2] in candidate
				if a_in and b_in and c_in:
					a_index = candidate.index(indices[ind*3+0]) + bias
					b_index = candidate.index(indices[ind*3+1]) + bias
					c_index = candidate.index(indices[ind*3+2]) + bias
					f.write(str(a_index)+' '+str(b_index)+' '+str(c_index)+'\n')
					count += 1
					used.add(indices[ind*3+0])
					used.add(indices[ind*3+1])
					used.add(indices[ind*3+2])

	# print('Indices {}:'.format(camera_id), count)
	bias += len(candidate)

# For main.cpp
f               = open(json_file) # Must be Ground Truth
data            = json.load(f)
cameraList      = data["camera_list"]
with open('preparation.txt', 'w') as f:
	image_path = ''
	for x in (json_file.split('/')[:-1]):
		image_path += x + '/'
	f.write('#define FILEPATH "{}"\n'.format(image_path))
	
	temp = ''
	for i in range(len(chosen_cam)):
		if(i==len(chosen_cam)-1):
			temp += str(chosen_cam[i])
		else:
			temp += str(chosen_cam[i]) + ','
	f.write('const int chosen_cam [] = {' + temp + '};\n')
