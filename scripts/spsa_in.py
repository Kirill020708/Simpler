def parse_file(filename):
    results = []

    count=0
    stcCount=0

    with open('params.txt', 'w') as outfile:
        with open('../code/tunables.h', 'r') as file:
            for line in file:
                line = line.strip()
                if not line or not line.startswith('#define'):
                    continue
                
                count += 1
                content = line[8:].strip()
                
                tokens = content.split()
                #print(tokens)

                lb = tokens[3]
                rb = tokens[4]
                Cend = (float(rb)-float(lb))/20
                try:
                    Cend = float(tokens[5])
                except:
                    pass

                isltc=False
                if '*' in tokens:
                    isltc = True
                else:
                    stcCount+=1
                    outfile.write(f"{tokens[0]}, int, {tokens[1]}, {lb}, {rb}, {Cend}, 0.002\n")

                #print(tokens)

    print('Total number of params:', count)
    print('Total number of stc params:', stcCount)

    return results

# Usage
parsed_data = parse_file('a.txt')
for item in parsed_data:
    print(item)