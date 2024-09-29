project('RedditChat', 'c',
  version: '0.1.0',
  default_options: ['c_std=c17', 'warning_level=3']
)

# Compiler options
cc = meson.get_compiler('c')
if cc.get_id() == 'msvc'
  add_project_arguments('/W4', language: 'c')
else
  add_project_arguments('-Wpedantic', language: 'c')
endif



# Add cJSON dependency
curl_dep = subproject('curl').get_variable('curl_dep')
cjson_dep = subproject('cjson').get_variable('libcjson_dep')


# Source files
matrix_src = files(
  'src/matrix/models/login_response.c',
  'src/matrix/models/messages.c',
  'src/matrix/matrix_client.c',
)

reddit_src = files(
  'src/reddit/base64.c',
  'src/reddit/reddit.c',
)

utils_src = files(
  'src/utils/http.c',
  'src/utils/ini_utils.c',
)

sources = [
  'src/main.c',
] + matrix_src + reddit_src + utils_src

# Include directories
inc_dirs = include_directories('include')

# Executable
executable_name = 'RedditChat'
executable = executable(executable_name, sources,
  dependencies: [curl_dep, cjson_dep],
  include_directories: inc_dirs
)

# Platform-specific settings
if host_machine.system() == 'windows'
  # Post-build command to copy DLL (if needed)
  # meson.add_install_script('copy_dll.py', 
  #   meson.current_build_dir() / 'subprojects/cjson/cjson.dll',
  #   meson.current_build_dir() / executable_name
  # )
endif