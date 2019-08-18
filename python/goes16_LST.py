import os
from netCDF4 import Dataset # netCDF library

filename = '../../data_using_netcdf/GOES/OR_ABI-L2-LSTC-M6_G16_s20192300301568_e20192300304341_c20192300305136.nc'

file = Dataset(filename) # open local netCDF GOES file
