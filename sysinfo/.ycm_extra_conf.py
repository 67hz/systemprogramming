from distutils.sysconfig import get_python_inc
import platform
import os
import subprocess
import ycm_core
flags = [
    '-x',
    'c',
    '-g',
    '-std=c99',
    '-Wall',
    '-Wextra',
    '-Werror',
]

DIR_OF_THIS_SCRIPT = os.path.abspath( os.path.dirname( __file__ ) )
DIR_OF_THIRD_PARTY = os.path.join( DIR_OF_THIS_SCRIPT, 'builddir' )
SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ]
compilation_database_folder = DIR_OF_THIS_SCRIPT

if os.path.exists( compilation_database_folder ):
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None

def IsHeaderFile( filename ):
  extension = os.path.splitext( filename )[ 1 ]
  return extension in [ '.h', '.hxx', '.hpp', '.hh' ]


def FindCorrespondingSourceFile( filename ):
  if IsHeaderFile( filename ):
    basename = os.path.splitext( filename )[ 0 ]
    for extension in SOURCE_EXTENSIONS:
      replacement_file = basename + extension
      if os.path.exists( replacement_file ):
        return replacement_file
  return filename

def Settings( ** kwargs):
  if kwargs[ 'language' ] == 'cfamily':
    filename = FindCorrespondingSourceFile( kwargs[ 'filename' ] )

    if not database:
      return {
        'flags': flags,
        'include_paths_relative_to_dir': DIR_OF_THIS_SCRIPT,
        'override_filename': filename
      }

    compilation_info = database.GetCompilationInfoForFile( filename )
    if not compilation_info.compiler_flags_:
      return {}

    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object.
    final_flags = list( compilation_info.compiler_flags_ )

    return {
      'flags': final_flags,
      'include_paths_relative_to_dir': compilation_info.compiler_working_dir_,
      'override_filename': filename
    }
  return {}


# compilation db
#cc -x c -std=c99 -Wall -Wextra -Werror `pkg-config --cflags gtk+-3.0` main.c -o main `pkg-config --libs gtk+-3.0`
