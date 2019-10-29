/** $lic$
 * Copyright (C) 2012-2015 by Massachusetts Institute of Technology
 * Copyright (C) 2010-2013 by The Board of Trustees of Stanford University
 *
 * This file is part of zsim.
 *
 * zsim is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * If you use this software in your research, we request that you reference
 * the zsim paper ("ZSim: Fast and Accurate Microarchitectural Simulation of
 * Thousand-Core Systems", Sanchez and Kozyrakis, ISCA-40, June 2013) as the
 * source of the simulator in any publications that use this software, and that
 * you send us a citation of your work.
 *
 * zsim is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "cache_arrays.h"
#include "hash.h"
#include "visualize.h"
#include "repl_policies.h"

/* Set-associative array implementation */
extern int index_ref[32768];
extern int index_ref_sub[32768];
extern int ref_perf[32768];
extern int sizes[32768][3];
extern int size;
extern char* data_array; // = (char*)malloc(32768 * 64);
extern char* data_array_sub; // = (char*)malloc(32768 * 64);
extern char* data_array_perf; // = (char*)malloc(32768 * 64);
Address record[1024];
extern char* data_array_orig;//[327680 * 64];
Line evicted_line;
extern int max_size;
extern int compress_size;
extern int compress_size_sub;
extern int compress_size_orig;
extern int uncompress_size;
extern int flip_compress_size;
extern int flip_compress_size_sub;
extern int flip_compress_size_orig;
extern int l1;
extern int initialized[32768][3];
int memAccess = 0;
extern int occupied[32768][3];
FILE *ability_file = fopen("best_senario.txt", "w");
FILE *compressibility = fopen("compressibility.txt", "w");
SetAssocArray::SetAssocArray(uint32_t _numLines, uint32_t _assoc, ReplPolicy* _rp, HashFamily* _hf, const char* _cname) : rp(_rp), hf(_hf), numLines(_numLines), assoc(_assoc), cname(_cname)  {
    array = gm_calloc<Address>(numLines);
    numSets = numLines/assoc;
    setMask = numSets - 1;
    assert_msg(isPow2(numSets), "must have a power of 2 # sets, but you specified %d", numSets);
}

// read data
int32_t SetAssocArray::lookup(const Address lineAddr, const MemReq* req, bool updateReplacement) {
	// hash family deciding the mapping of the set
    uint32_t set = hf->hash(0, lineAddr) & setMask;
	// the index of the first elem in the test
    uint32_t first = set*assoc;
    for (uint32_t id = first; id < first + assoc; id++) {
        if (array[id] ==  lineAddr) {
			//printf("looking up %lld\n", data_array[id]);
			// update the fields for different replacement policies
            if (updateReplacement) rp->update(id, req);
            return id;
        }
    }
    return -1;
}

uint32_t SetAssocArray::preinsert(const Address lineAddr, const MemReq* req, Address* wbLineAddr) { //TODO: Give out valid bit of wb cand?
    uint32_t set = hf->hash(0, lineAddr) & setMask;
    uint32_t first = set*assoc;

    uint32_t candidate = rp->rankCands(req, SetAssocCands(first, first+assoc));

    // get the address of candidate
	*wbLineAddr = array[candidate];
	//printf("Finding %u: %lld to replace\n", candidate, data_array[candidate]);
    return candidate;
}

void SetAssocArray::postinsert(const Address lineAddr, const MemReq* req, uint32_t candidate) {
    rp->replaced(candidate);
    
    Line line = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
    Line line_sub = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
    Line line_orig = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   
	if(l1 == 1){
		line_orig = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		for(int i = 0 ; i < 64; i ++){
				line_orig.Byte[i]= (*((char*) ((array[candidate] << 6) + i)));
		}
		sizes[candidate][0] = countLine(line_orig);
		if(index_ref[candidate]){
		    line = cTol_orig(index_ref[candidate]);
		    line = xxor(line, line_orig);
		    for(int i = 0; i < 64; i++){
		        data_array[candidate * 64 + i] = (char) line.Byte[i];
		    }
		    sizes[candidate][1] = countLine(line);
		  }
		else
		    line = compare(line_orig, candidate);
		if(index_ref_sub[candidate]){
			line_sub = cTol_orig(index_ref_sub[candidate]);
			line_sub = sub(line_sub, line_orig);
			for(int i = 0; i < 64; i++){
			    data_array_sub[candidate * 64 + i] = (char) line_sub.Byte[i];
			}
			sizes[candidate][2] = countLine(line_sub);
	  	}
	  	else
		    line_sub = compare_sub(line_orig, candidate);
		
		line = compress(line);
		line_sub = compress(line_sub);
		line_orig = compress(line_orig);
		
		calcRate(64, countLine(line), countFlip(line));
		//calcRate_sub(64, countLine(line_sub), countFlip(line_sub));
		calcRate_orig(64, countLine(line_orig), countFlip(line_orig));
    }
	
    for(int i = 0; i < 1024; i++){
		if(record[i] == candidate){
			record[i] = lineAddr;
			break;
		}
    }
    array[candidate] = lineAddr;
	
    rp->update(candidate, req);
}

// read data
int32_t SetAssocArray::lookupL2(const Address lineAddr, const MemReq* req, bool updateReplacement) {
	// hash family deciding the mapping of the set
    uint32_t set = hf->hash(0, lineAddr) & setMask;
	// the index of the first elem in the test
    uint32_t first = set*assoc;
    for (uint32_t id = first; id < first + assoc; id++) {
        if (array[id] ==  lineAddr) {
			// update the fields for different replacement policies
            if (updateReplacement) rp->update(id, req);
            return id;
        }
    }
    return -1;
}

uint32_t SetAssocArray::preinsertL2(const Address lineAddr, const MemReq* req, Address* wbLineAddr) {
    uint32_t set = hf->hash(0, lineAddr) & setMask;
    uint32_t first = set*assoc;

    uint32_t candidate = rp->rankCands(req, SetAssocCands(first, first+assoc));

    // get the address of candidate
	*wbLineAddr = array[candidate];
    return candidate;
}

Line* SetAssocArray::refresh(const Address lineAddr, const MemReq* req, uint32_t candidate) { 
    rp->replaced(candidate);
    Line * line = (Line *) malloc(sizeof(Line));
    if(array[candidate]){
		for(int i = 0 ; i < 64; i++){
            line->Byte[i] =  *((char*) ((array[candidate] << 6) + i));
        }
    }
    return line;
}

void SetAssocArray::postinsertL2(const Address lineAddr, const MemReq* req, uint32_t candidate) {
    memAccess ++;
    rp->replaced(candidate);
    Line former_line = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // get line from memory
    for(int i = 0 ; i < 64; i ++){
        former_line.Byte[i]= (*((char*) ((array[candidate] << 6) + i)));
    }
    array[candidate] = lineAddr;
    // initialize each parameters
    Line line = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
    Line line_sub = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
    Line line_orig = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};     

    printf("We are in postinsertL2 for addr %ld\n", lineAddr);
    // get line from memory
    for(int i = 0 ; i < 64; i ++){
            line_orig.Byte[i]= (*((char*) ((array[candidate] << 6) + i)));
    }
    sizes[candidate][0] = countLine(line_orig);
    // on eviction
    if(index_ref[candidate]){
        printf("ocean address %ld\n", lineAddr);
        Line reference = cTol_orig(index_ref[candidate]);
        if(countLine(xxor(reference, line_orig)) > countLine(line_orig)){
            line = xxor(reference, line_orig);
            for(int i = 0; i < 64; i++){
                data_array[candidate * 64 + i] = (char) line.Byte[i];
                data_array[index_ref[candidate] * 64 + i] = (char) line.Byte[i];
            }
        }
        else
            line = compare(line_orig, candidate);
        sizes[candidate][1] = countLine(line);
    }
    // simple insertion
    else{
        printf("island address %ld\n", lineAddr);
        line = compare(line_orig, candidate);
    }
    // sub: not in use
    if(index_ref_sub[candidate]){
        line_sub = cTol_orig(index_ref_sub[candidate]);
        line_sub = sub(line_sub, line_orig);
        for(int i = 0; i < 64; i++){
            data_array_sub[candidate * 64 + i] = (char) line_sub.Byte[i];
        }
        sizes[candidate][2] = countLine(line_sub);
  	}
  	else
     line_sub = compare_sub(line_orig, candidate);
   /*  
   if(memAccess % 100 == 0){
     occupied[candidate][2] = 0;
     occupied[ref_perf[candidate]][2] = 0;
     Line perf = compare_perfect(line_orig, candidate);
   }
		
    calcRate_raw(64, countLine(line), countFlip(line));
    calcRate_orig_raw(64, countLine(line_orig), countFlip(line_orig));
*/
    // apply BDI on lines
    line = compress(line);
    line_sub = compress(line_sub);
    line_orig = compress(line_orig);
    
    // print result to files
    calcRate(64, countLine(line), countFlip(line));
    calcRate_orig(64, countLine(line_orig), countFlip(line_orig));
/*   
   if(memAccess % 100 == 0){
     printf("here.\n");
     fprintf(ability_file, "best rate:%1f ", best_compression_rate());
     fprintf(compressibility, "best possible rate: %1f", best_compression_rate());
   }*/
    	
    rp->update(candidate, req);
    TestPrint(candidate);
}

/* ZCache implementation */

ZArray::ZArray(uint32_t _numLines, uint32_t _ways, uint32_t _candidates, ReplPolicy* _rp, HashFamily* _hf) //(int _size, int _lineSize, int _assoc, int _zassoc, ReplacementPolicy<T>* _rp, int _hashType)
    : rp(_rp), hf(_hf), numLines(_numLines), ways(_ways), cands(_candidates)
{
    assert_msg(ways > 1, "zcaches need >=2 ways to work");
    assert_msg(cands >= ways, "candidates < ways does not make sense in a zcache");
    assert_msg(numLines % ways == 0, "number of lines is not a multiple of ways");

    //Populate secondary parameters
    numSets = numLines/ways;
    assert_msg(isPow2(numSets), "must have a power of 2 # sets, but you specified %d", numSets);
    setMask = numSets - 1;

    lookupArray = gm_calloc<uint32_t>(numLines);
    array = gm_calloc<Address>(numLines);
    for (uint32_t i = 0; i < numLines; i++) {
        lookupArray[i] = i;  // start with a linear mapping; with swaps, it'll get progressively scrambled
    }
    swapArray = gm_calloc<uint32_t>(cands/ways + 2);  // conservative upper bound (tight within 2 ways)
}

void ZArray::initStats(AggregateStat* parentStat) {
    AggregateStat* objStats = new AggregateStat();
    objStats->init("array", "ZArray stats");
    statSwaps.init("swaps", "Block swaps in replacement process");
    objStats->append(&statSwaps);
    parentStat->append(objStats);
}

int32_t ZArray::lookup(const Address lineAddr, const MemReq* req, bool updateReplacement) {
    /* Be defensive: If the line is 0, panic instead of asserting. Now this can
     * only happen on a segfault in the main program, but when we move to full
     * system, phy page 0 might be used, and this will hit us in a very subtle
     * way if we don't check.
     */
    if (unlikely(!lineAddr)) panic("ZArray::lookup called with lineAddr==0 -- your app just segfaulted");

    for (uint32_t w = 0; w < ways; w++) {
        uint32_t lineId = lookupArray[w*numSets + (hf->hash(w, lineAddr) & setMask)];
        if (array[lineId] == lineAddr) {
            if (updateReplacement) {
                rp->update(lineId, req);
            }
            return lineId;
        }
    }
    return -1;
}

uint32_t ZArray::preinsert(const Address lineAddr, const MemReq* req, Address* wbLineAddr) {
    ZWalkInfo candidates[cands + ways]; //extra ways entries to avoid checking on every expansion

    bool all_valid = true;
    uint32_t fringeStart = 0;
    uint32_t numCandidates = ways; //seeds

    //info("Replacement for incoming 0x%lx", lineAddr);

    //Seeds
    for (uint32_t w = 0; w < ways; w++) {
        uint32_t pos = w*numSets + (hf->hash(w, lineAddr) & setMask);
        uint32_t lineId = lookupArray[pos];
        candidates[w].set(pos, lineId, -1);
        all_valid &= (array[lineId] != 0);
        //info("Seed Candidate %d addr 0x%lx pos %d lineId %d", w, array[lineId], pos, lineId);
    }

    //Expand fringe in BFS fashion
    while (numCandidates < cands && all_valid) {
        uint32_t fringeId = candidates[fringeStart].lineId;
        Address fringeAddr = array[fringeId];
        assert(fringeAddr);
        for (uint32_t w = 0; w < ways; w++) {
            uint32_t hval = hf->hash(w, fringeAddr) & setMask;
            uint32_t pos = w*numSets + hval;
            uint32_t lineId = lookupArray[pos];

            // Logically, you want to do this...
#if 0
            if (lineId != fringeId) {
                //info("Candidate %d way %d addr 0x%lx pos %d lineId %d parent %d", numCandidates, w, array[lineId], pos, lineId, fringeStart);
                candidates[numCandidates++].set(pos, lineId, (int32_t)fringeStart);
                all_valid &= (array[lineId] != 0);
            }
#endif
            // But this compiles as a branch and ILP sucks (this data-dependent branch is long-latency and mispredicted often)
            // Logically though, this is just checking for whether we're revisiting ourselves, so we can eliminate the branch as follows:
            candidates[numCandidates].set(pos, lineId, (int32_t)fringeStart);
            all_valid &= (array[lineId] != 0);  // no problem, if lineId == fringeId the line's already valid, so no harm done
            numCandidates += (lineId != fringeId); // if lineId == fringeId, the cand we just wrote will be overwritten
        }
        fringeStart++;
    }

    //Get best candidate (NOTE: This could be folded in the code above, but it's messy since we can expand more than zassoc elements)
    assert(!all_valid || numCandidates >= cands);
    numCandidates = (numCandidates > cands)? cands : numCandidates;

    //info("Using %d candidates, all_valid=%d", numCandidates, all_valid);

    uint32_t bestCandidate = rp->rankCands(req, ZCands(&candidates[0], &candidates[numCandidates]));
    assert(bestCandidate < numLines);

    //Fill in swap array

    //Get the *minimum* index of cands that matches lineId. We need the minimum in case there are loops (rare, but possible)
    uint32_t minIdx = -1;
    for (uint32_t ii = 0; ii < numCandidates; ii++) {
        if (bestCandidate == candidates[ii].lineId) {
            minIdx = ii;
            break;
        }
    }
    assert(minIdx >= 0);
    //info("Best candidate is %d lineId %d", minIdx, bestCandidate);

    lastCandIdx = minIdx; //used by timing simulation code to schedule array accesses

    int32_t idx = minIdx;
    uint32_t swapIdx = 0;
    while (idx >= 0) {
        swapArray[swapIdx++] = candidates[idx].pos;
        idx = candidates[idx].parentIdx;
    }
    swapArrayLen = swapIdx;
    assert(swapArrayLen > 0);

    //Write address of line we're replacing
    *wbLineAddr = array[bestCandidate];

    return bestCandidate;
}

void ZArray::postinsert(const Address lineAddr, const MemReq* req, uint32_t candidate) {
    //We do the swaps in lookupArray, the array stays the same
    assert(lookupArray[swapArray[0]] == candidate);
    for (uint32_t i = 0; i < swapArrayLen-1; i++) {
        //info("Moving position %d (lineId %d) <- %d (lineId %d)", swapArray[i], lookupArray[swapArray[i]], swapArray[i+1], lookupArray[swapArray[i+1]]);
        lookupArray[swapArray[i]] = lookupArray[swapArray[i+1]];
    }
    lookupArray[swapArray[swapArrayLen-1]] = candidate; //note that in preinsert() we walk the array backwards when populating swapArray, so the last elem is where the new line goes
    //info("Inserting lineId %d in position %d", candidate, swapArray[swapArrayLen-1]);

    rp->replaced(candidate);
    array[candidate] = lineAddr;
    rp->update(candidate, req);

    statSwaps.inc(swapArrayLen-1);
}

int32_t ZArray::lookupL2(const Address lineAddr, const MemReq* req, bool updateReplacement) {
    return 0;
}
uint32_t ZArray::preinsertL2(const Address lineAddr, const MemReq* req, Address* wbLineAddr) {
    return 0;
}

void ZArray::postinsertL2(const Address lineAddr, const MemReq* req, uint32_t candidate) {
}
