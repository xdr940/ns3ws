from path import Path
import os
#inputs
work_space = "./ns3ws/src"
comp_subdir_list = [
    'myfirst',
    # 'mysec',
    # 'my4',
    # 'my3',
    # 'my7'
    #'my_wifi',
    # 'my6',
    # 'mythird',
    #'withdirs'
    ]


def list_files_rec(path, list_name):  #recurve to list
    for file in os.listdir(path):  
        file_path = os.path.join(path, file)  
        if os.path.isdir(file_path):  
            list_files_rec(file_path, list_name)  
        elif file_path.endswith(".cc"):  
            list_name.append(file_path)
        else:
            continue 


def add_scratch_programs(bld):
    all_modules = [mod[len("ns3-"):] for mod in bld.env['NS3_ENABLED_MODULES'] + bld.env['NS3_ENABLED_CONTRIBUTED_MODULES']]

    try:
        for filename in os.listdir(work_space):
            if filename.startswith('.') or filename == 'CVS':
                continue
            if os.path.isdir(os.path.join(work_space, filename)):
                obj = bld.create_ns3_program(filename, all_modules)
                obj.path = obj.path.find_dir(work_space).find_dir(filename)
                obj.source = obj.path.ant_glob('*.cc')
                obj.target = filename
                obj.name = obj.target
                obj.install_path = None
            elif filename.endswith(".cc"):
                name = filename[:-len(".cc")]
                obj = bld.create_ns3_program(name, all_modules)
                obj.path = obj.path.find_dir(work_space)
                obj.source = filename
                obj.target = name
                obj.name = obj.target
                obj.install_path = None
            print("-> file name:    {}, ".format(filename),type(filename))
            print("-> obj path:     {},  ".format(obj.path),type(obj.path))
            print("-> obj source:   {},  ".format(obj.source),type(obj.source))
            print("-> obj target:   {},  ".format(obj.target),type(obj.target))
            print("\n\n")
    except OSError:
        return


def add_scratch_programs1(bld):
    '''
    add comp_subdir_list
    '''
    all_modules = [mod[len("ns3-"):] for mod in bld.env['NS3_ENABLED_MODULES'] + bld.env['NS3_ENABLED_CONTRIBUTED_MODULES']]

    try:
        for filename in os.listdir(work_space):
            if filename not in comp_subdir_list:
                continue
            if filename.startswith('.') or filename == 'CVS':
                continue
            if os.path.isdir(os.path.join(work_space, filename)):
                obj = bld.create_ns3_program(filename, all_modules)

                obj.path = obj.path.find_dir(work_space).find_dir(filename)#abspath in target dir, selfclass!
    
                obj.source = obj.path.ant_glob('*.cc')#return lsit of node3!


                obj.target = filename
                obj.name = obj.target
                obj.install_path = None
            elif filename.endswith(".cc"):
                name = filename[:-len(".cc")]
                obj = bld.create_ns3_program(name, all_modules)
                obj.path = obj.path.find_dir('scratch')
                obj.source = filename
                obj.target = name
                obj.name = obj.target
                obj.install_path = None
            else:
                continue
            print("-> file name:    {}, ".format(filename),type(filename))
            print("-> obj path:     {},  ".format(obj.path),type(obj.path))
            print("-> obj source 0:   {},  ".format(obj.source[0]),type(obj.source[0]))
            print("-> obj target:   {},  ".format(obj.target),type(obj.target))
            print("\n\n")
    except OSError:
        return


def add_scratch_programs2(bld):
    #add multiple layer content index
    #add list of dirs for comp
    # obj path :dir name
    # obj source :files list,include every *.cc file
    # obj target: excute file name
    all_modules = [mod[len("ns3-"):] for mod in bld.env['NS3_ENABLED_MODULES'] + bld.env['NS3_ENABLED_CONTRIBUTED_MODULES']]

    try:
        work_space_p = Path(work_space)
        if not work_space_p.exists():
            print("work space error")
        else:
            print("-> work space:{}".format(work_space))
        
        ccfiles=[]
        list_files_rec(work_space,ccfiles)
        # print("ccfiles:{}".format(ccfiles))
        dirs = work_space_p.dirs()
        dirs.sort()
        files = work_space_p.files()
        files.sort()


        for dir in dirs:
        # for filename in ccfiles:
           

            if dir.stem not in comp_subdir_list:
                continue
            print("dir-name {}".format(dir))
            ccfiles=[]
            list_files_rec(dir,ccfiles)
            for filename in ccfiles:
                filename = Path(filename)
                print("-->:{}".format(filename))
                obj = bld.create_ns3_program(filename, all_modules)

                obj.target =str(dir.stem)
                obj.name = obj.target
                
                obj.path = obj.path.find_dir("ns3ws/src/{}".format(dir.stem))#output,abspath,subdir


                obj.source = str(filename)
               
                # if os.path.exists(str(filename.abspath())):
                    # print('ok')
                obj.install_path = None
               

            print("-> filename{}, {}".format(filename,type(filename)))
            print("-> obj path{}, {}".format(obj.path,type(obj.path)))
            print("-> obj source{}, {}".format(obj.source,type(obj.source)))
            print("-> obj.target {}, {}".format(obj.target,type(obj.target)))
            print("\n\n")
            
    except OSError:
        return