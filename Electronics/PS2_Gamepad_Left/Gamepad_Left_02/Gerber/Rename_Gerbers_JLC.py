import glob
import os
import zipfile
import shutil

project_name = 'nix'

# Make a list of .gbr and .drl files in the current directory.

path = os.path.dirname(__file__)
path_absolute = path.replace(os.sep, '/')

gerbers = glob.glob(os.path.join(path, "*.gbr"))
gerbers.extend(glob.glob(os.path.join(path, "*.drl")))

# File renaming rules.
gerber_types = [
    {'from': '-B_Silkscreen.gbr',   'to': '.bottomsilkscreen.gbr'},
    {'from': '-B_Mask.gbr',         'to': '.bottomsoldermask.gbr'},
    {'from': '-B_Cu.gbr',           'to': '.bottomlayer.gbr'},
    {'from': '-Inner1_Cu.gbr',      'to': '.internalplane1.gbr'},
    {'from': '-Inner2_Cu.gbr',      'to': '.internalplane2.gbr'},
    {'from': '-Inner3_Cu.gbr',      'to': '.internalplane3.gbr'},
    {'from': '-Inner4_Cu.gbr',      'to': '.internalplane4.gbr'},
    {'from': '-F_Cu.gbr',           'to': '.toplayer.gbr'},
    {'from': '-F_Mask.gbr',         'to': '.topsoldermask.gbr'},
    {'from': '-F_Silkscreen.gbr',   'to': '.topsilkscreen.gbr'},
    {'from': '-Edge_Cuts.gbr',      'to': '.boardoutline.gbr'},
    {'from': '-B_Paste.gbr',        'to': '.bottompaste.gbr'},
    {'from': '-F_Paste.gbr',        'to': '.toppaste.gbr'},
    {'from': '-NPTH.drl',           'to': '.holes_npth.drl'},
    {'from': '-PTH.drl',            'to': '.drills_pth.drl'},
]

if len(gerbers) == 0:
    print ("No files found with this extensions: ")
    for type in gerber_types:
        print ("\t" + type['from'] + "")
    print ("\nEND of script.")
else:
    print ("Load files:")
    for g in gerbers:
        print (g)
        for t in gerber_types:
            #extract the project name for the zip file
            project_name = os.path.basename(g[:-len(t['from'])]) + ".zip"
    z = zipfile.ZipFile(os.path.join(path, project_name), 'w')
    print ("\n",)
    
    print ("Start renaming:")
    # Rename files depending upon their names.
    for g in gerbers:
        for t in gerber_types:
            if g.endswith(t['from']):
                # Strip the 'from' string from the old name and append the 'to' string to make the new name.
                new_g = g[:-len(t['from'])] + t['to']

                # Remove any existing file having the new name.
                print ("\t" + g + " -> " + new_g)
                try:
                    os.remove(new_g)
                except:
                    # An exception occurred because the file we tried to remove probably didn't exist.
                    pass

                #copy the old file and assign the new name
                shutil.copy(g, new_g)
                
                #create a zip file containing the renamed files
                z.write(new_g, new_g.split(os.sep)[-1])

                #remove the renamed files from the directory, as they are in the zip file
                try:
                    os.remove(new_g)
                except:
                    #An exception occurred because the file we tried to remove probably didn't exist.
                    pass

                break
