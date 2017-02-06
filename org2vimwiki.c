/* This is an attempt to implement an org2vimwiki converter in C.
 * Specification: 
 * Take any line that starts with the regexp ^*+, and replace all the
 * *'s with ='s, and put an equal amount of ='s at the end.
 *
 * I would like to implement this in perl or sed, but C seems like a 
 * funny choice, and a good way to exercise my I/O handling.
 *
 * The main problem: This way of checking for '\n' doesn't work when 
 * the first line of the file is a header. This is a weakness of using
 * character reading, rather than full lines.
 *
 * The problem is that if I switch to using getline, or fgets, I have 
 * to put an upper limit on line lengths. But that's just what I don't 
 * want to assume for these files.
 *
 * Licensed under GPL v3+
 */

#include <stdio.h>
#include <error.h>
#include <stdlib.h>

void org2wiki(char *orgFilename, char *wikiFilename)
{
    int c, nc;
    FILE *orgFile, *vimwikiFile;
    
    /* Open the org mode file. We open r+ so we can use ungetc. */
    if ((orgFile = fopen(orgFilename, "r+")) == NULL) {
        perror(orgFilename);
        exit(2);
    }
    
    if ((vimwikiFile = fopen(wikiFilename, "w")) == NULL) {
        perror(wikiFilename);
        exit(2);
    }

    // Flags
    int headerline = 0; // Are we in a header line?
    int headerflag = 0; // Are we in the starting header?
    int headercount = 0; // How deep are we?

    /* This is a bit hacky. I want to use character manipulation so I
     * don't have to assume that the lines are a certain length. But
     * character manipulation presupposes we have a newline to test
     * if the next line is a header.
     *
     * The solution is to just see if the first character is a *, since
     * if it is, we have a first-line header. We then manually set the
     * relevant flags.
     */
    if ((c = fgetc(orgFile)) == '*') {
        headerline = headerflag = 1;
        headercount = 0;
    }
    ungetc(c, orgFile);

    // Main loop
    while ((c = fgetc(orgFile)) != EOF) {
        // Newline is a sign to test both for start and end of header.
        if (c == '\n') {
            // Check if we are at the end of a header.
            if (headerline == 1 && headerflag == 0) {
                // If we are, print a space and then the trailing ='s.
                fputc(' ', vimwikiFile);
                while(headercount--) {
                    fputc('=', vimwikiFile);
                }
                headerline = 0;
            }
            // Check if we are at the start of a header.
            if ((nc = fgetc(orgFile)) == '*') {
                headerline = headerflag = 1;
                headercount = 0;
            } 
            ungetc(nc, orgFile);

            // Can't forget to still add the newline:
            fputc(c, vimwikiFile);

        // Check if we are in the start of a header:
        } else if (headerflag == 1 && c == '*') {
            // If we are, start printing *'s.
            fputc('=', vimwikiFile);
            headercount++;
            if ((nc = fgetc(orgFile)) != '*')
                headerflag = 0;
            ungetc(nc, orgFile);
        
        // If we get this far, output the relevant character.
        } else {
            fputc(c, vimwikiFile);
        }
    }

    fclose(orgFile);
    fclose(vimwikiFile);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s orginputfile vimwikioutputfile\n", argv[0]);
        exit(1);
    }

    org2wiki(argv[1], argv[2]);
    printf("org2wiki conversion complete!\n");
    return 0;
}
