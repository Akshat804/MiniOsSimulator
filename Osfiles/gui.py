import tkinter as tk
from tkinter import scrolledtext
import subprocess
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import re
import sys
import networkx as nx

# ✅ Cross-platform executable
OS_CMD = ["os.exe"] if sys.platform.startswith("win") else ["./os"]

# ================= RUN OS =================
def run_os():
    command = input_box.get("1.0", tk.END)

    process = subprocess.Popen(
        OS_CMD,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    output, _ = process.communicate(command + "\nexit\n")

    result_box.delete(1.0, tk.END)
    result_box.insert(tk.END, output)

    parse_metrics(output)


# ================= METRICS =================
avg_wt = None
avg_tat = None

def parse_metrics(output):
    global avg_wt, avg_tat

    wt = re.search(r'Average Waiting Time = ([\d\.]+)', output)
    tat = re.search(r'Average Turnaround Time = ([\d\.]+)', output)

    if wt:
        avg_wt = float(wt.group(1))
    if tat:
        avg_tat = float(tat.group(1))


def show_graph():
    if avg_wt is None:
        result_box.insert(tk.END, "\n❌ Run scheduling first\n")
        return

    plt.bar(["Waiting", "Turnaround"], [avg_wt, avg_tat])
    plt.title("Performance Metrics")
    plt.show()


# ================= COMPARE =================
def compare_algorithms():
    base_input = input_box.get("1.0", tk.END)

    if not base_input.strip():
        result_box.insert(tk.END, "\n❌ Enter processes first\n")
        return

    algos = ["fcfs", "rr 2", "mlfq"]
    results = []

    for algo in algos:
        cmd = base_input + f"\nschedule {algo}\n"

        process = subprocess.Popen(
            OS_CMD,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            text=True
        )

        output, _ = process.communicate(cmd + "exit\n")

        match = re.search(r'Average Waiting Time = ([\d\.]+)', output)

        results.append(float(match.group(1)) if match else 0)

    plt.bar(["FCFS", "RR", "MLFQ"], results)
    plt.title("Algorithm Comparison")
    plt.show()


# ================= GANTT =================
def extract_gantt(output):
    match = re.search(r'Gantt Chart:\n(.+)', output, re.DOTALL)
    if not match:
        return []

    return re.findall(r'P\d+', match.group(1))


def animate_gantt():
    command = input_box.get("1.0", tk.END)

    process = subprocess.Popen(
        OS_CMD,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text=True
    )

    output, _ = process.communicate(command + "\nexit\n")

    gantt = extract_gantt(output)

    if not gantt:
        result_box.insert(tk.END, "\n❌ No Gantt data\n")
        return

    fig, ax = plt.subplots()

    # 🔥 assign unique colors
    colors = {}
    color_list = ["red", "blue", "green", "orange", "purple"]

    def get_color(p):
        if p not in colors:
            colors[p] = color_list[len(colors) % len(color_list)]
        return colors[p]

    def update(frame):
        ax.clear()
        ax.set_xlim(0, len(gantt))
        ax.set_ylim(0, 1)
        ax.set_yticks([])

        for i in range(frame):
            color = get_color(gantt[i])
            ax.barh(0, 1, left=i, color=color)
            ax.text(i + 0.3, 0, gantt[i], color="white")

    animation.FuncAnimation(fig, update, frames=len(gantt)+1, interval=500)
    plt.title("Gantt Animation")
    plt.show()


# ================= MEMORY =================
def extract_memory(output):
    frames_list = []
    status_list = []
    replaced_list = []

    lines = output.split("\n")

    for line in lines:
        if "[" in line:
            frame = re.findall(r'\[(.*?)\]', line)
            if frame:
                frames_list.append(frame[0].split())

            if "HIT" in line:
                status_list.append("HIT")
                replaced_list.append(None)

            elif "FAULT" in line:
                status_list.append("FAULT")

                # 🔥 FIXED regex
                rep = re.search(r'(?:Removed Page|replaced)\s*(\d+)', line)

                if rep:
                    replaced_list.append(rep.group(1))
                else:
                    replaced_list.append(None)

    return frames_list, status_list, replaced_list

def animate_memory():
    command = input_box.get("1.0", tk.END)

    process = subprocess.Popen(
        OS_CMD,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text=True
    )

    output, _ = process.communicate(command + "\nexit\n")

    frames, status_list, replaced_list = extract_memory(output)

    if not frames:
        result_box.insert(tk.END, "\n❌ No memory data\n")
        return

    fig, ax = plt.subplots()

    total_frames = min(len(frames), len(status_list), len(replaced_list))

    def update(i):
        ax.clear()

        frame = frames[i]
        status = status_list[i]
        replaced = replaced_list[i]

        ax.set_title(f"Step {i+1} - {status}")
        ax.set_xlim(0, len(frame))
        ax.set_ylim(0, 1)
        ax.set_xticks([])
        ax.set_yticks([])

        for j, val in enumerate(frame):

            if val == "-":
                color = "lightgray"

            elif replaced is not None and str(val) == str(replaced):
                color = "red"

            else:
                color = "skyblue"

            ax.barh(0, 1, left=j, color=color)
            ax.text(j + 0.4, 0, val, ha='center')

    ani = animation.FuncAnimation(
        fig, update, frames=total_frames, interval=800, repeat=False
    )

    plt.show()

def visualize_deadlock():
    text = input_box.get("1.0", tk.END)

    lines = text.strip().split("\n")

    edges = []

    for line in lines:
        parts = line.split()
        if len(parts) == 2:
            u, v = parts
            edges.append((f"P{u}", f"P{v}"))

    if not edges:
        result_box.insert(tk.END, "\n❌ Enter edges like: 0 1\n")
        return

    G = nx.DiGraph()
    G.add_edges_from(edges)

    pos = nx.spring_layout(G)

    plt.figure()

    # 🔥 detect cycle
    try:
        cycle = nx.find_cycle(G, orientation='original')
        cycle_edges = [(u, v) for u, v, _ in cycle]
    except:
        cycle_edges = []

    # draw nodes
    nx.draw(G, pos, with_labels=True, node_color="lightblue", node_size=2000)

    # 🔴 highlight cycle
    if cycle_edges:
        nx.draw_networkx_edges(G, pos, edgelist=cycle_edges, edge_color="red", width=3)
        plt.title("Deadlock Detected (Cycle Highlighted)")
    else:
        plt.title("No Deadlock")

    plt.show()
# ================= GUI =================
root = tk.Tk()
root.title("OS Simulator Dashboard")
root.geometry("900x600")

left_frame = tk.Frame(root)
left_frame.pack(side=tk.LEFT, padx=10)

tk.Label(left_frame, text="Commands").pack()

input_box = scrolledtext.ScrolledText(left_frame, width=40, height=15)
input_box.pack()

tk.Button(left_frame, text="Run OS", command=run_os).pack(pady=5)
tk.Button(left_frame, text="Show Graph", command=show_graph).pack(pady=5)
tk.Button(left_frame, text="Compare Algorithms", command=compare_algorithms).pack(pady=5)
tk.Button(left_frame, text="Animate Gantt", command=animate_gantt).pack(pady=5)
tk.Button(left_frame, text="Animate Memory", command=animate_memory).pack(pady=5)
tk.Button(left_frame, text="Visualize Deadlock", command=visualize_deadlock).pack(pady=5)

right_frame = tk.Frame(root)
right_frame.pack(side=tk.RIGHT, padx=10)

tk.Label(right_frame, text="Output").pack()

result_box = scrolledtext.ScrolledText(right_frame, width=50, height=25)
result_box.pack()

root.mainloop()