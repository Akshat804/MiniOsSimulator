#include "memory.h"

MemoryManager::MemoryManager(int framesCount) {
    numFrames = framesCount;
    frames.assign(numFrames, -1);
    referenceBit.assign(numFrames, false);
    pointer = 0;
}

int MemoryManager::accessPage(int pid, int page) {

    // 🔹 TLB HIT
    if (TLB.count(page)) {
        cout << "TLB HIT for page " << page << endl;
        return TLB[page];
    }

    // 🔹 Memory HIT
    for (int i = 0; i < numFrames; i++) {
        if (frames[i] == page) {
            cout << "Memory HIT for page " << page << endl;
            referenceBit[i] = true;
            TLB[page] = i;
            return i;
        }
    }

    // 🔴 PAGE FAULT
    cout << "PAGE FAULT for page " << page << endl;

    int frame = clockReplace(page);
    TLB[page] = frame;

    return frame;
}

int MemoryManager::clockReplace(int page) {

    while (true) {
        if (!referenceBit[pointer]) {
            frames[pointer] = page;
            referenceBit[pointer] = true;

            int replaced = pointer;
            pointer = (pointer + 1) % numFrames;
            return replaced;
        }

        referenceBit[pointer] = false;
        pointer = (pointer + 1) % numFrames;
    }
}

void MemoryManager::printFrames() {
    cout << "Frames: ";
    for (int i = 0; i < numFrames; i++) {
        cout << frames[i] << " ";
    }
    cout << endl;
}