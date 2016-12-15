#include "cachelab.h"
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define LATEST_LRU 0x3f3f3f3f

typedef struct {
	int valid;
	int tag;
	int lru;	
}line;

typedef struct {
	line* lines;
}group;

typedef struct {
	int nr_group;
	int nr_line;
	group* groups;
}sim_cache;

void get_opt(int argc, char* argv[],int *s,int *E,int *b,int *v, char* file)
{
	int temp;
	while((temp = getopt(argc, argv, "hvs:E:b:t:")) != -1)
	{
		switch(temp)
		{
			case 's': *s = atoi(optarg); break;
			case 'E': *E = atoi(optarg); break;
			case 'b': *b = atoi(optarg); break;
			case 'v': *v=1; break;
			case 't':
				 strcpy(file, optarg);
				 break;
			case 'h':
			default :
				 printf("RTFM\n");
				 exit(0);
		}
	}
}

void creat_cache(sim_cache *cache, int s, int E, int b)
{
	cache->nr_group = (1 << s);
	cache->nr_line = E;
	cache->groups = (group *)(malloc(cache->nr_group * sizeof(group)));
	int i;
	for(i=0; i < cache->nr_group; i++)
	{
		cache->groups[i].lines = (line *)(malloc(E * sizeof(line)));
		int j;
		for(j=0; j < E; j++)
		{
			cache->groups[i].lines[j].valid = 0;
			cache->groups[i].lines[j].lru = 0;
		}
	}
}

uint32_t get_tag(uint32_t addr, int s, int b)
{
	return addr >> (s + b);
}

uint32_t get_group(uint32_t addr, int s, int b)
{
	addr >>= b;
	addr &= ((1 << s) - 1);
	return addr;
}

uint32_t find_replace(sim_cache *cache, int group)
{
	int min_line = 0;
	int min_val = LATEST_LRU;
	int i;
	for(i=0; i < cache->nr_line; i++)
	{
		if(cache->groups[group].lines[i].lru < min_val)
		{
			min_line = i;
			min_val = cache->groups[group].lines[i].lru;
		}
	}
	return min_line;
}

void update_lru(sim_cache *cache, int group, int line)
{
	int i;
	for(i=0; i < cache->nr_line; i++)
		cache->groups[group].lines[i].lru--;
	cache->groups[group].lines[line].lru = LATEST_LRU;
}

int is_hit(sim_cache *cache, int group, int tag)
{
	int i;
	for(i=0; i < cache->nr_line; i++)
		if(cache->groups[group].lines[i].valid && cache->groups[group].lines[i].tag == tag)
		{
			update_lru(cache, group, i);
			return 1;
		}
	return 0;
}

int is_full(sim_cache *cache, int group, int tag)
{
	int i;
	for(i=0; i < cache->nr_line; i++)
		if(cache->groups[group].lines[i].valid == 0)
			break;
	return i;
}

int update_cache(sim_cache *cache, int group, int tag)
{
	int full;
	int line = is_full(cache, group, tag);
	if(line < cache->nr_line)
	{
		cache->groups[group].lines[line].valid = 1;
		cache->groups[group].lines[line].tag = tag;
		update_lru(cache, group, line);
		full = 0;
	}
	else
	{	
		line = find_replace(cache, group);
		cache->groups[group].lines[line].valid = 1;
		cache->groups[group].lines[line].tag = tag;
		update_lru(cache, group, line);
		full = 1;
	}
	return full;
}
		
int hit, miss, eviction;

void L(sim_cache *cache, int group, int tag, int v)
{
	if(is_hit(cache, group, tag))
	{
		hit++;
		if(v)
			printf("hit ");
	}
	else
	{
		miss++;
		if(v)
			printf("miss ");
		if(update_cache(cache, group, tag))
		{
			eviction++;
			if(v)
				printf("eviction ");
		}
	}
}


void S(sim_cache *cache, int group, int tag, int v)
{
	L(cache, group, tag, v);
}

void M(sim_cache *cache, int group, int tag, int v)
{
	L(cache, group, tag, v);
	S(cache, group, tag, v);
}

int main(int argc, char* argv[])
{
	int s, E, b, v = 0;
	char tracefile[1000];
	char opt[5];
	uint32_t addr;
	int size;
	sim_cache cache;

	miss = hit = eviction = 0;
	
	get_opt(argc, argv, &s, &E, &b, &v, tracefile);
	creat_cache(&cache, s, E, b);
	FILE *trace = fopen(tracefile, "r");
	while(fscanf(trace, "%s %x,%d", opt, &addr, &size) == 3)
	{
		if(strcmp(opt, "I") == 0)
			continue;
		int group = get_group(addr, s, b);
		int tag = get_tag(addr, s, b);
		if(v)
			printf("%s %x,%d", opt, addr, size);
		if(strcmp(opt, "L") == 0)
			L(&cache, group, tag, v);
		else if(strcmp(opt, "S") == 0)
			S(&cache, group, tag, v);
		else
			M(&cache, group, tag, v);
	}
	if(v)
		printf("\n");
	printSummary(hit, miss, eviction);
    	return 0;
}
