def parse_file(filename):
    results = []

    count=0
    stcCount=0

    params = open('params.txt', 'w')
    ucispsa = open('../code/ucispsa.h', 'w')
    tunespsa = open('../code/tunablesSPSAmode.h', 'w')


    ucispsaOut = '#pragma once\n\nvoid printSPSAparams() {\n    cout <<\n'

    ucispsaIn = ''

    with open('../code/tunables.h', 'r') as file:
        for line in file:
            line = line.strip()
            if not line or not line.startswith('#define'):
                continue
            
            count += 1
            content = line[8:].strip()
            
            tokens = content.split()
            #print(tokens)
            name = tokens[0]
            value = tokens[1]
            lb = tokens[3]
            rb = tokens[4]
            Cend = (float(rb)-float(lb))/20
            try:
                Cend = float(tokens[5])
            except:
                pass

            ucispsaOut+=f'    "option name {name} type spin default {value} min {lb} max {rb}\\n\"\n'
            ucispsaIn +=f'    if (name == \"{name}\") {name} = value;\n';

            tunespsa.write(f'int {name} = {value};\n')

            isltc=False
            if '*' in tokens:
                isltc = True
            else:
                stcCount+=1
                params.write(f"{tokens[0]}, int, {tokens[1]}, {lb}, {rb}, {Cend}, 0.002\n")

            #print(tokens)
    ucispsaOut+=';\n}\n\nvoid setParam(string name, int value) {\n'
    ucispsaOut += ucispsaIn
    ucispsaOut += '}\n'

    ucispsa.write(ucispsaOut)

    print('Total number of params:', count)
    print('Total number of stc params:', stcCount)

    return results

# Usage
parsed_data = parse_file('a.txt')
for item in parsed_data:
    print(item)