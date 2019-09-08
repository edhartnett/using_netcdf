# This program demonstrates some netCDF fill values.
import netCDF4
import time

t0 = time.time()

formats = ('NETCDF3_CLASSIC', 'NETCDF4')
#formats = ('NETCDF4', 'NETCDF3_CLASSIC')
#fill_modes = ('off', 'on')
fill_modes = ('on', 'off')

# Create a file.
for format in formats:
    for fill_mode in fill_modes:
        root = netCDF4.Dataset('fill_values_'+ fill_mode + '_' + format + '.nc',
                               'w', format=format)
        if fill_mode == 'on':
            root.set_fill_on()
        else:
            root.set_fill_off()
        dim1 = root.createDimension('dim1', 10000)
        dim2 = root.createDimension('dim2', 10000)
        var = root.createVariable('var1', 'f4', ('dim1', 'dim2'))
        root.close()
        t1 = time.time()
        total = t1 - t0
        print format + ' fill mode ' + fill_mode + ' time ' + str(total) + ' s'
