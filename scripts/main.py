# this should be run during the build process when things are changed. It generates tables
# that are used by the program

import os
import colour as c



def main():
    generate_brightness_tables()
    

def generate_brightness_tables(a_side_colour: c.Color, b_side_colour: c.Color):
    ...
    
if __name__ == "__main__":
    main()