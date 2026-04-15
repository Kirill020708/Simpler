def parse_file(filename):
    results = []

    count=0
    stcCount=0

    out = open('defineout.txt', 'w')


    ucispsaOut = '#pragma once\n\nvoid printSPSAparams() {\n    cout <<\n'

    ucispsaIn = ''

    tunedValues = {}

    with open('paramsOut.txt', 'r') as file:
        for line in file:
            line = line.strip()
            name, value = line.strip().split(',')
            name = name.strip()
            value = value.strip()
            tunedValues[name] = value

    with open('../code/tunables.h', 'r') as file:
        for line in file:
            line = line.strip()
            if not line or not line.startswith('#define'):
                out.write(line+'\n')
                continue
            
            content = line[8:].strip()
            
            tokens = content.split()
            name = tokens[0]
            value = tokens[1]

            if name in tunedValues:
                value = tunedValues[name]

            suffix = ' '.join(tokens[2::])

            out.write(f'#define {name} {value} {suffix}\n')

    return results

# Usage
parsed_data = parse_file('paramsOut.txt')
for item in parsed_data:
    print(item)