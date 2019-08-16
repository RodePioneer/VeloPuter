#!/usr/bin/env python

import argparse

def main():
    parser = argparse.ArgumentParser(description="Setup of the veloputer")
    parser.add_argument("-m", "--model", metavar="MODEL", 
                        choices=("QUATTRO", "DF"),
                        default="QUATTRO",
                        help="Basic model, determines default settings. Values STRADA, QUILTJE, QUATTRO, or DF")
    parser.add_argument("-c", "--battery", metavar="TYPE",
                        choices=("LiPo", "LiFePO4"), default="LiPo",
                        help="Battery chemistry, determines discharge curve and protection logic. Values LiPo or LiFePO4")
    parser.add_argument("-b", "--blinkers", metavar="N", type=int, 
                        help="Number of times the blinkers will blink when cornering. Default 5", 
                        default=5)
    parser.add_argument("-C", "--cassette", metavar="N", type=float, default=None,
                        nargs="+", help="Number of teeth on the rear cassette. Default 11-36 + Schlumpf")
    parser.add_argument("-f", "--chainring", metavar="N", type=int, default=70,
                        help="Number of teeth on the fron chainring. Default 70")
    parser.add_argument("-w", "--wheel", metavar="LENGTH", type=int, default=1540,
                        help="Circumference of the front wheels in mm. Default 1540 (40-406)")
    parser.add_argument("-W", "--rear-wheel", metavar="LENGTH", type=int, default=None,
                        help="Circumference of the rear wheel in mm. Default same as front wheels.")
    
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
    
    contents.append("/* define cassette */")
    if args.cassette is None:
        args.cassette = [11.0, 13.0, 15.0, 17.0, 19.0, 22.0, 25.0, 28.0, 32.0, 36.0,
                         255.0, 255.0, 37.5, 42.5, 47.5, 55.0, 62.5, 70.0, 80.0, 90.0]
    contents.append("#define VP_CASSETTE_LENGHT %d" % (len(args.cassette), ))
    contents.append("#define VP_CASSETTE_VALUES %s" % (",".join(["%.1f" % (v,) for v in args.cassette]), ))
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
    
    contents.append("#endif /* #if VELOPUTER_CONFIG_H */")
    contents.append("")
    
    ref = open("VeloPuter_Config.h", "w")
    with ref:
        ref.write("\n".join(contents))


if __name__ == "__main__":
    main()
