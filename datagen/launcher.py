#!/usr/bin/env python3
import subprocess
import argparse
import os
import sys
import glob
import platform
import random


def aggregate_stats():
    # Find all stat files

    stat_files = sorted(glob.glob("datagenFiles/stats/stat*.txt"))
    
    if not stat_files:
        print("No stat files found!")
        return
    
    print(f"Found {len(stat_files)} stat files")
    print("-" * 50)
    
    # Initialize aggregates
    total_games = 0
    total_positions = 0
    total_games_per_sec = 0.0
    total_positions_per_sec = 0.0
    positions_per_game_list = []
    total_time_list = []
    
    # Process each file
    for file in stat_files:
        with open(file, 'r') as f:
            data = {}
            for line in f:
                if ':' in line:
                    key, value = line.strip().split(': ')
                    data[key] = value
            
            games = int(data.get('games_number', 0))
            positions = int(data.get('positions_saved', 0))
            pos_per_game = float(data.get('positions_per_game', 0))
            total_time = float(data.get('total_time', 0))
            games_per_sec = float(data.get('games_per_sec', 0))
            positions_per_sec = float(data.get('positions_per_sec', 0))
            
            total_games += games
            total_positions += positions
            total_games_per_sec += games_per_sec
            total_positions_per_sec += positions_per_sec
            positions_per_game_list.append(pos_per_game)
            total_time_list.append(total_time)
    
    # Calculate means
    mean_positions_per_game = sum(positions_per_game_list) / len(positions_per_game_list)
    mean_total_time = sum(total_time_list) / len(total_time_list)
    
    # Print results
    print(f"Total games: {total_games}")
    print(f"Total positions: {total_positions}")
    print(f"Mean positions per game: {mean_positions_per_game:.1f}")
    print(f"Mean total time: {mean_total_time:.1f}")
    print(f"Total games per sec: {total_games_per_sec:.1f}")
    print(f"Total positions per sec: {total_positions_per_sec:.1f}\n")


def combineData():
    data_files = sorted(glob.glob("datagenFiles/data/data*.vf"))

    # Concatenate into single file
    with open("datagenFiles/data/data.vf", "wb") as outfile:
        for file in data_files:
            with open(file, "rb") as infile:
                outfile.write(infile.read())

    print(f"Combined {len(data_files)} files into data.vf\n")


def cleanData():
    data_files = sorted(glob.glob("datagenFiles/data/data*.vf"))

    for file in data_files:
        if not file.endswith("data.vf"):
            os.remove(file)

    print(f"Cleaned {len(data_files) - 1} files\n")

current_path = ''

def terminateProcesses():
    global current_path
    open(current_path+"/datagenFiles/exit.txt", "w").close()
    '''system = platform.system()
    
    if system == "Windows":
        cmd = ["taskkill", "/F", "/IM", "Simpler.exe"]
    else:  # Linux or macOS
        cmd = ["pkill", "-9", "Simpler"]
    
    try:
        subprocess.run(cmd, capture_output=True, text=True)
        print(f"Killed Simpler processes on {system}")
    except:
        print("No Simpler processes found or command failed")'''


def main():
    global current_path

    parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter,
    epilog='''Example:

    python3 launcher.py \\
        --threads 4 \\
        --file "./../Simpler" \\
        --softnodes 5000 \\
        --hardnodes 100000 \\
        --games 10000 \\
        --resignMoveCount 3 \\
        --resignScore 1000 \\
        --drawMoveCount 8 \\
        --minDrawMoveCount 34 \\
        --drawScore 20 \\
        --launch

    python3 launcher.py --stats --combine --clean --terminate
    '''
    )

    parser.add_argument('--homePath', type=str, default=os.getcwd())

    parser.add_argument('--threads', type=int, default=1)
    parser.add_argument('--file', type=str, default='')
    parser.add_argument('--softnodes', type=int, default=5000)
    parser.add_argument('--hardnodes', type=int, default=100000)
    parser.add_argument('--games', type=int, default=1000000)
    parser.add_argument('--resignMoveCount', type=int, default=3)
    parser.add_argument('--resignScore', type=int, default=1000)
    parser.add_argument('--drawMoveCount', type=int, default=8)
    parser.add_argument('--minDrawMoveCount', type=int, default=34)
    parser.add_argument('--drawScore', type=int, default=20)

    parser.add_argument('--launch', action='store_true')

    parser.add_argument('--stats', action='store_true')

    parser.add_argument('--combine', action='store_true')

    parser.add_argument('--clean', action='store_true')

    parser.add_argument('--terminate', action='store_true')
    
    args = parser.parse_args()


    current_path = args.homePath
    os.makedirs(current_path+"/datagenFiles", exist_ok=True)
    os.makedirs(current_path+"/datagenFiles/data", exist_ok=True)
    os.makedirs(current_path+"/datagenFiles/stats", exist_ok=True)

    if args.stats:
        aggregate_stats()

    if args.combine:
        combineData()

    if args.clean:
        cleanData()

    if args.terminate:
        terminateProcesses()
    
    if args.launch:

        
        processes = []
        for i in range(args.threads):
            thread_id = i + 1
            process = subprocess.Popen(
                [args.file],
                stdin=subprocess.PIPE,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                text=True
            )
            seed = random.randint(0,10000000)
            input_template = f"""datagen seed {seed} outputDir {current_path} softnodes {args.softnodes} hardnodes {args.hardnodes} games {args.games} id {thread_id} resignMoveCount {args.resignMoveCount} resignScore {args.resignScore} drawMoveCount {args.drawMoveCount} minDrawMoveCount {args.minDrawMoveCount} drawScore {args.drawScore}"""
            process.stdin.write(input_template)
            process.stdin.close()
            processes.append(process)

if __name__ == "__main__":
    main()



'''
python3 launcher.py \
    --threads 4 \
    --file "./../Simpler" \
    --softnodes 5000 \
    --hardnodes 100000 \
    --games 10000 \
    --resignMoveCount 3 \
    --resignScore 1000 \
    --drawMoveCount 8 \
    --minDrawMoveCount 34 \
    --drawScore 20 \
    --launch


python3 launcher.py --stats --combine --clean --terminate
'''