#!/usr/bin/env python
########################################################################
#                                                                      #
#       Toy Model for Pronton Charge Confusion with Dense Layer        #
#                      version 6.2      09.2018                        #
#                                                                      # 
########################################################################


########################## packages ####################################
from __future__ import division
import numpy as np
import math
import json
import collections
import matplotlib.pyplot as plt 
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.colors import LogNorm
from keras.layers import Activation, Dropout, Flatten, Dense  
from keras.layers import Dense, Conv2D, MaxPooling2D  
from keras.models import Sequential, Model, model_from_json   
from keras.optimizers import SGD, RMSprop, Adam
from keras.preprocessing.image import ImageDataGenerator   
from keras.utils import np_utils
from keras import initializers
from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw
plt.switch_backend('agg')

########################## Free Parameters #############################
binnumber=150

########################## Properties Label ############################
MvAresult=16
TrdLogLikelihoodRatioProtonHeliumTracker=17
######################### Test events ##################################
testset1 = np.load('/hpcwork/jara0052/sichen/analysis_6.2/ISS/147-1000GeV/transferdata/positive/147_175GeV/pattern0/positive_147_175_pattern_0.npy')
testset2 = np.load('/hpcwork/jara0052/sichen/analysis_6.2/ISS/147-1000GeV/transferdata/negative/147_175GeV/pattern0/negative_147_175_pattern_0.npy')

######################### Ecal Bdt #####################################
#testset1=testset1[np.where((testset1[:,-5]>-2) & (testset1[:,-5]<0.5))[0],:]
#testset2=testset2[np.where((testset2[:,-5]>-2) & (testset2[:,-5]<0.5))[0],:]

######################### trd He/proton #####################################
#testset1=testset1[np.where(testset1[:,TrdLogLikelihoodRatioProtonHeliumTracker]<0.3)[0],:]
#testset2=testset2[np.where(testset2[:,TrdLogLikelihoodRatioProtonHeliumTracker]<0.3)[0],:]

##################################################################################
testset=np.r_[testset1,testset2]
testsetMvA=testset[:,MvAresult] ## get the CCProton MvA results
testset=np.expand_dims(testset, axis=2)
inputlist=np.arange(1,16)
testset=testset[:,inputlist,:]
testset=np.fabs(testset)
features=15

############## model and compile with sequential model #################
import CNN_models
model = CNN_models.VGG16(features)
model.load_weights('VGG16.h5')
y_pred = model.predict(testset)

######################## save template data ################################
fplot=file('data/plot_ISS_positive_rigidity.npy','wb')
np.save(fplot, np.transpose( [y_pred[0:testset1.shape[0],1], testset1[:,0]] ))
fplot.close()
fplot=file('data/plot_ISS_negative_rigidity.npy','wb')
np.save(fplot, np.transpose( [y_pred[testset1.shape[0]:(testset1.shape[0]+testset2.shape[0]),1], testset2[:,0]] ))
fplot.close()

fplot=file('data/plot_ISS_positive_rigidity_CCMVA.npy','wb')
np.save(fplot, np.transpose( [testsetMvA[0:testset1.shape[0]] ,testset1[:,0]] ))
fplot.close()
fplot=file('data/plot_ISS_negative_rigidity_CCMVA.npy','wb')
np.save(fplot, np.transpose( [testsetMvA[testset1.shape[0]:(testset1.shape[0]+testset2.shape[0])], testset2[:,0]] ))
fplot.close()




'''
########################## NN Prediction ##################################

plt.figure(figsize=(18,18))
plt.hist(y_pred[0:testset1.shape[0],1],bins=binnumber,range=(0,1),log=True,alpha=0.5,label='Positive',facecolor='blue',edgecolor='black' )
plt.hist(y_pred[testset1.shape[0]:y_pred.shape[0],1],bins=binnumber,range=(0,1),log=True,alpha=0.5,label='Negative',facecolor='green',edgecolor='black' )
plt.xticks(fontsize=20)
plt.yticks(fontsize=20)
plt.xlabel('Estimator $_{CC}$',fontsize=22)
plt.ylabel('Count',fontsize=22)
plt.legend(loc='upper center',fontsize=30)
plt.savefig('plot/ML_test_ISS.png')

########################  MvA results ##############################
plt.figure(figsize=(18,18))
plt.hist(testsetMvA[0:testset1.shape[0]],binnumber,range=(0,1),log=True, alpha=0.5,label='Positive',facecolor='blue',edgecolor='black'  )
plt.hist(testsetMvA[testset1.shape[0]:y_pred.shape[0]],binnumber,range=(0,1),log=True, alpha=0.5,label='Negative',facecolor='green',edgecolor='black'  ) 
plt.xlabel('Estimator $_{CC}$',fontsize=30)
plt.ylabel('Count',fontsize=30)
plt.xticks(fontsize=25)
plt.yticks(fontsize=25)
plt.legend(loc='upper center',fontsize=30)
plt.savefig('plot/CCMVA_test_ISS.png')

#########################################################################
plt.figure(figsize=(18,18))
plt.hist2d(y_pred[0:testset1.shape[0],1],testset1[:,0],bins=binnumber, range=[[0, 1], [0, 1.5]],alpha=0.5,norm=LogNorm() )
plt.hist2d(y_pred[testset1.shape[0]:y_pred.shape[0],1],testset2[:,0],bins=binnumber,range=[[0, 1], [0, 1.5]],alpha=0.5,norm=LogNorm())
plt.xticks(fontsize=20)
plt.yticks(fontsize=20)
plt.xlabel(r'$\Lambda_{CC}$',fontsize=22)
plt.ylabel(r'$\Lambda_{TRD}$',fontsize=22)
plt.legend(loc='upper center',fontsize=30)
plt.colorbar()
plt.savefig('plot/ML_ISS_2d.png')

plt.figure(figsize=(18,18))
plt.hist2d(testsetMvA[0:testset1.shape[0]],testset1[:,0],bins=binnumber,range=[[0, 1], [0, 1.5]],alpha=0.5,norm=LogNorm() )
plt.hist2d(testsetMvA[testset1.shape[0]:y_pred.shape[0]],testset2[:,0],bins=binnumber,range=[[0, 1], [0, 1.5]],alpha=0.5,norm=LogNorm() )
plt.xlabel(r'$\Lambda_{CC}$',fontsize=30)
plt.ylabel(r'$\Lambda_{TRD}$',fontsize=30)
plt.xticks(fontsize=25)
plt.yticks(fontsize=25)
plt.legend(loc='upper center',fontsize=30)
plt.colorbar()
plt.savefig('plot/CCMVA_ISS_2d.png')

_x = np.arange(0,1,0.05)
_y = np.arange(0,1.5,0.075)
_xx, _yy = np.meshgrid(_x, _y)
x, y = _xx.ravel(), _yy.ravel()
width = 0.05
depth = 0.075
top=((np.histogram2d(testsetMvA[0:testset1.shape[0]],testset1[:,0],bins=20,range=[[0, 1], [0, 1.5]])[0]).T).ravel()
top2=((np.histogram2d(testsetMvA[testset1.shape[0]:y_pred.shape[0]],testset2[:,0],bins=20,range=[[0, 1], [0, 1.5]])[0]).T).ravel()
bottom = np.zeros_like(top)
fig=plt.figure(figsize=(8, 8))
ax1=fig.add_subplot(111,projection='3d')
ax1.bar3d(x, y, bottom, width, depth, np.log10(top), shade=True)
ax1.bar3d(x, y, bottom, width, depth, np.log10(top2), shade=True)
plt.xlabel(r'$\Lambda_{CC}$' )
plt.ylabel(r'$\Lambda_{TRD}$' )
#ax1.set_zscale('log')
#plt.show()
ax1.view_init(30, 45)
plt.savefig('plot/CCMVA_ISS_2dbar.png')

_x = np.arange(0,1,0.05)
_y = np.arange(0,1.5,0.075)
_xx, _yy = np.meshgrid(_x, _y)
x, y = _xx.ravel(), _yy.ravel()
width = 0.05
depth = 0.075
top=((np.histogram2d(y_pred[0:testset1.shape[0],1],testset1[:,0],bins=20,range=[[0, 1], [0, 1.5]])[0]).T).ravel()
top2=((np.histogram2d(y_pred[testset1.shape[0]:y_pred.shape[0],1],testset2[:,0],bins=20,range=[[0, 1], [0, 1.5]])[0]).T).ravel()
bottom = np.zeros_like(top)
fig=plt.figure(figsize=(8, 8))
ax1=fig.add_subplot(111,projection='3d')
ax1.bar3d(x, y, bottom, width, depth, np.log10(top), shade=True)
ax1.bar3d(x, y, bottom, width, depth, np.log10(top2), shade=True)
plt.xlabel(r'$\Lambda_{CC}$' )
plt.ylabel(r'$\Lambda_{TRD}$' )
#ax1.set_zscale('log')
#plt.show()
ax1.view_init(30, 45)
plt.savefig('plot/ML_ISS_2dbar.png')
'''

