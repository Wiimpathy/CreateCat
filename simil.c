#include <stdio.h>
#include <string.h>
 

int levenshtein(const char *s, const char *t)
{
	int ls = strlen(s), lt = strlen(t);
	int d[ls + 1][lt + 1];
 
	for (int i = 0; i <= ls; i++)
		for (int j = 0; j <= lt; j++)
			d[i][j] = -1;
 
	int dist(int i, int j) {
		if (d[i][j] >= 0) return d[i][j];
 
		int x;
		if (i == ls)
			x = lt - j;
		else if (j == lt)
			x = ls - i;
		else if (s[i] == t[j])
			x = dist(i + 1, j + 1);
		else {
			x = dist(i + 1, j + 1);
 
			int y;
			if ((y = dist(i, j + 1)) < x) x = y;
			if ((y = dist(i + 1, j)) < x) x = y;
			x++;
		}
		return d[i][j] = x;
	}
	return dist(0, 0);
}


int main(int argc, char *argv[])
{
	char ligne[364];
	size_t len = 0;
	ssize_t read;

	FILE *fp=fopen(argv[2], "rt");

	if(fp == NULL) 
	{
		perror("Error opening file");
		return(-1);
	}

	while (fgets(ligne,364,fp) != NULL)  
	{
		int dist=levenshtein(argv[1],  ligne);

		if(strchr(argv[1], '1') || strchr(argv[1], '2'))
		{
			if(dist <= 1)
			{
				printf("%s",  ligne );
			}
		}
		else
		{
			if(dist <= 4)
			{
				printf("%s",  ligne );
			}
		}
	}

	fclose(fp);

	return 0;
}
