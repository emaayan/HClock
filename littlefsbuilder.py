Import("env")
env.Replace( MKFSTOOL=env.get("PROJECT_DIR") + '/mklittlefs' )