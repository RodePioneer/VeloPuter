#!/usr/bin/env python

import argparse
from functools import partial

def range_type(astr, min=0, max=101):
    value = int(astr)
    if min<= value <= max:
        return value
    else:
        raise argparse.ArgumentTypeError('value not in range %s-%s'%(min,max))

def main():
    parser = argparse.ArgumentParser(description="Setup of the veloputer")
    parser.add_argument("-m", "--model", metavar="MODEL", 
                        choices=("QUATTRO", "DF"),
                        default="QUATTRO",
                        help="Basic model, determines default settings. Values QUATTRO, or DF")
    parser.add_argument("-c", "--battery", metavar="TYPE",
                        choices=("LiPo", "LiFePO4"), default="LiPo",
                        help="Battery chemistry, determines discharge curve and protection logic. Values LiPo or LiFePO4")
    parser.add_argument("-b", "--blinkers", metavar="N", type=int, 
                        help="Number of times the blinkers will blink when cornering. Default 5", 
                        default=5)
    parser.add_argument("-f", "--chainring", metavar="N", type=int, default=70,
                        help="Number of teeth on the fron chainring. Default 70")
    parser.add_argument("-w", "--wheel", metavar="LENGTH", type=int, default=1540,
                        help="Circumference of the front wheels in mm. Default 1540 (40-406)")
    parser.add_argument("-W", "--rear-wheel", metavar="LENGTH", type=int, default=None,
                        help="Circumference of the rear wheel in mm. Default same as front wheels.")
    parser.add_argument("-d", "--dimintens", metavar="VALUE", type=partial(range_type, min=0, max=255), default=96, 
                        help="Intensity of dimmed lights.")
    
    args = parser.parse_args()
    
    contents = []
    contents.append("""/* Header with configuration of this VeloPuter.
 * Note that this file is generated with the VeloPuter-setup.py script.
 * If you want to make changes, you probably want to run the script again.
 *
 * This file is not under version control because it is generated 
 */


/* Ensure this header is only loaded once */ """)
    
    contents.append("#if !defined(VELOPUTER_CONFIG_H)")
    contents.append("#define VELOPUTER_CONFIG_H")
    contents.append("")
    
    model_map = {"QUATTRO":"QUATRO", "DF":"ICB_DF"}
    contents.append("/* define model of the velomobile */")
    contents.append("#define %s" % (model_map[args.model], ))
    contents.append("")
    
    contents.append("/* define battery type */")
    contents.append("#define BATTERY_%s" % (args.battery.upper(), ))
    contents.append("")
    
    contents.append("/* define blink-count */")
    contents.append("#define VP_BLINK_COUNT %d" % (args.blinkers, ))
    contents.append("")
        
    contents.append("/* define chainring */")
    contents.append("#define VP_CHAINRING %d" % (args.chainring, ))
    contents.append("")
    
    contents.append("/* define front wheel circumference */")
    contents.append("#define VP_FRONTWHEEL %d" % (args.wheel, ))
    contents.append("")
    
    contents.append("/* define rear wheel circumference */")
    contents.append("#define VP_REARWHEEL %d" % (args.rear_wheel if args.rear_wheel is not None else args.wheel, ))
    contents.append("")
    
    contents.append("/* intensity of dimmed lights */")
    contents.append("#define VP_DIMMED_INTENSITY %d" % (args.dimintens,))
    contents.append("")
    
    contents.append("#endif /* #if VELOPUTER_CONFIG_H */")
    contents.append("")
    
    ref = open("VeloPuter_Config.h", "w")
    with ref:
        ref.write("\n".join(contents))


if __name__ == "__main__":
    main()
