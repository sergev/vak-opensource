--------------------------------------------------------------------------

  2910 Translator to read "bits" format and create a VHDL test vector file

  Developed on Nov 1, 1991 by :
                                Indraneel Ghosh,
                                CADLAB,
                                Univ. of Calif. , Irvine, CA 92717

--------------------------------------------------------------------------

/* Generation of test vectors */
#include <stdio.h>
#include <string.h>

#define INITIAL_COMMENT_LINES 11
#define NO_OF_PORTS 22
#define MAX_LINE_SIZE 100
#define MAX_BIT_VECTOR_SIZE 20

#define NAME_LINE INITIAL_COMMENT_LINES
#define DIR_LINE INITIAL_COMMENT_LINES + 1
#define TYPE_LINE INITIAL_COMMENT_LINES + 2

main(argc,argv)

  int argc;
  char *argv[];

{

  FILE *infptr;
  FILE *outfptr;

  char *c;
  char *a;
  char *temp;

  char var[MAX_BIT_VECTOR_SIZE];
  char line[MAX_LINE_SIZE];

  int line_cnt;
  int field_cnt;
  int vec_cnt;
  int assert_cnt;
  int printed_vec_no;
  int instr_cnt;

  struct signal {
    char name[10];
    char port_type[4];
    char data_type[3];
  } ports[NO_OF_PORTS];

  printf ("Hi.\n");

  /* Check arguments */

  if (argc != 3)
    {
      fprintf(stderr,"Usage: %s <vectorfile> <vhdlfile>\n", argv[0]);
      fflush(stderr);
      exit(1);
    }

  /* Open Files */

  infptr = fopen(argv[1], "r");  
  outfptr = fopen(argv[2], "w"); 
  
  fprintf(outfptr,"--------------------------------------------------------");
  fprintf(outfptr,"------------------\n\n");
  fprintf(outfptr,"-- SIMULATION TEST VECTORS FOR AM 2910 MICROPROGRAM");
  fprintf(outfptr," SEQUENCER\n\n");
  fprintf(outfptr,"-- THE MODELS WERE SIMULATED ON THE ZYCAD (VERSION 1.0a)");
  fprintf(outfptr," SIMULATOR.\n\n");
  fprintf(outfptr,"--  Developed on Jan 1, 1992 by :\n");
  fprintf(outfptr,"--                                Indraneel Ghosh,\n");
  fprintf(outfptr,"--                                CADLAB,\n");
  fprintf(outfptr,"--                                Univ. of Calif. ,");
  fprintf(outfptr," Irvine.\n\n");
  fprintf(outfptr,"--------------------------------------------------------");
  fprintf(outfptr,"------------------\n\n");

fprintf(outfptr,"library ZYCAD;\n");
fprintf(outfptr,"use ZYCAD.types.all;\n");
fprintf(outfptr,"use work.MVL4_functions.all;\t--some binary functions\n");
fprintf(outfptr,"use work.synthesis_types.all;\t--hints for synthesis\n\n");

fprintf(outfptr,"entity E is\n");
fprintf(outfptr,"end;\n\n");

fprintf(outfptr,"architecture AA of E is\n");
fprintf(outfptr,"\t component am_func2910\n");
fprintf(outfptr,"\t  port (\n");
fprintf(outfptr,"\t\tI : in MVL4_VECTOR(3 downto 0);\n");
fprintf(outfptr,"\t\tCCEN_BAR : in MVL4;\n");
fprintf(outfptr,"\t\tCC_BAR : in MVL4;\n");
fprintf(outfptr,"\t\tRLD_BAR : in MVL4;\n");
fprintf(outfptr,"\t\tCI : in MVL4;\n");
fprintf(outfptr,"\t\tOEbar : in MVL4;\n");
fprintf(outfptr,"\t\tclk : in clock;\n");
fprintf(outfptr,"\t\tD : in MVL4_VECTOR(11 downto 0);\n");
fprintf(outfptr,"\t\tY : out MVL4_VECTOR(11 downto 0);\n");
fprintf(outfptr,"\t\tPL_BAR : out MVL4;\n");
fprintf(outfptr,"\t\tVECT_BAR : out MVL4;\n");
fprintf(outfptr,"\t\tMAP_BAR : out MVL4;\n");
fprintf(outfptr,"\t\tFULL_BAR : out MVL4\n");
fprintf(outfptr,"\t  );\n");
fprintf(outfptr,"\t end component;\n\n");
                                                                          
fprintf(outfptr,"\t\tsignal I : MVL4_VECTOR(3 downto 0);\n");
fprintf(outfptr,"\t\tsignal CCEN_BAR : MVL4;\n");
fprintf(outfptr,"\t\tsignal CC_BAR : MVL4;\n");
fprintf(outfptr,"\t\tsignal RLD_BAR : MVL4;\n");
fprintf(outfptr,"\t\tsignal CI : MVL4;\n");
fprintf(outfptr,"\t\tsignal OEbar : MVL4;\n");
fprintf(outfptr,"\t\tsignal clk : clock;\n");
fprintf(outfptr,"\t\tsignal D : MVL4_VECTOR(11 downto 0);\n");
fprintf(outfptr,"\t\tsignal Y : MVL4_VECTOR(11 downto 0);\n");
fprintf(outfptr,"\t\tsignal PL_BAR : MVL4;\n");
fprintf(outfptr,"\t\tsignal VECT_BAR : MVL4;\n");
fprintf(outfptr,"\t\tsignal MAP_BAR : MVL4;\n");
fprintf(outfptr,"\t\tsignal FULL_BAR : MVL4;\n\n");

fprintf(outfptr,"for all : am_func2910 use entity work.AM2910(AM2910);\n\n");

fprintf(outfptr,"begin\n\n");

fprintf(outfptr,"AM_FUNC1 : am_func2910 port map(\n");
fprintf(outfptr,"\t\tI,\n");                                                   
fprintf(outfptr,"\t\tCCEN_BAR,\n");
fprintf(outfptr,"\t\tCC_BAR,\n");
fprintf(outfptr,"\t\tRLD_BAR,\n");
fprintf(outfptr,"\t\tCI,\n");
fprintf(outfptr,"\t\tOEbar,\n");
fprintf(outfptr,"\t\tclk,\n");
fprintf(outfptr,"\t\tD,\n");
fprintf(outfptr,"\t\tY,\n");
fprintf(outfptr,"\t\tPL_BAR,\n");
fprintf(outfptr,"\t\tVECT_BAR,\n");
fprintf(outfptr,"\t\tMAP_BAR,\n");
fprintf(outfptr,"\t\tFULL_BAR \n");
fprintf(outfptr,"           );\n\n");

fprintf(outfptr,"process\n\n");

fprintf(outfptr,"begin\n\n");


  /* Initialize counters */

  line_cnt = 0;
  assert_cnt = 0;
  vec_cnt = 0;
  instr_cnt = 0;

  /* Start line-by-line processing */

  while((c =  fgets(line, 100, infptr)) != NULL) /* while not EOF, get a line */
    {
      
      /* Ignore the first few comment lines */

      if( (line_cnt < (INITIAL_COMMENT_LINES + 3) ) &&                                    (line_cnt >= INITIAL_COMMENT_LINES) )     
	{

	  /* Initialize port name(line0), port dir(line1), port type(line2) */

	  field_cnt = 0;

	  /* Start processing each item in the line, separated by ":" */

	  while((a = strchr(line, ':')) != NULL) /*till you reach end of line*/
	    {
	      
	      /* copy next item into "var". Make "line" point to */
	      /* the item after that.                            */

	      temp = a;          /* copy pointer to next ":" */
	      temp++;            /* take pointer to next location after ":" */
	      *a = '\0';         /* write "end of string" in place of ":" */
	      strcpy(var,line);  /* copy the portion before ":" into "var" */
	      strcpy(line,temp); /* copy the portion after ":" into "line" */

	      switch(line_cnt) {
	      case NAME_LINE:
		strcpy(ports[field_cnt].name,var); /* port names */
                break;
	      case DIR_LINE:
		strcpy(ports[field_cnt].port_type,var); /* port dir */
		break;
	      case TYPE_LINE:
		strcpy(ports[field_cnt].data_type,var); /* port type */
	      default:		
		break;
		}

	      field_cnt++;
	    }   
	}
      else if( line_cnt >= (INITIAL_COMMENT_LINES + 3) )
	{
	  /* Process the actual test vector lines */

	  if(line[0] == '*')
	    {
	      /* If the line is a comment line, print it verbatim */

	      fprintf(outfptr,"-- %s",line);
	    }
	  else
	    {

	      /* Reset the boolean variable to show that vec_no */
	      /* has not yet been printed for this line         */

 	      printed_vec_no = 0;
	      
	      /* Print some initial statements for each test vector */
	      
	      fprintf(outfptr,"--------------------------\n\n");
	      fprintf(outfptr,"clk <= '1'; --\t Cycle No: %i\n\n",instr_cnt);
	      instr_cnt++;
	      fprintf(outfptr,"wait for 1 ns;\n\n");

	      field_cnt = 0;

	      /* Start processing each item in the line, separated by ":" */
	      /* Each item is a signal value (in or out) */

	      while((a = strchr(line, ':')) != NULL) 
		{
		  /* copy next item into "var". Make "line" point to */
		  /* the item after that.                            */

		  temp = a;          /* copy pointer to next : */
		  temp++;            /* take pointer to next location after : */
		  *a = '\0';         /* write "end of string" in place of : */
		  strcpy(var,line);  /* copy the portion before : into "var" */
		  strcpy(line,temp); /* copy the portion after : into "line") */
		  
		  if (strcmp(ports[field_cnt].port_type,"in") == 0 )
		    {
		      /* If its an input port */
		      
		      if (var[0] != '-')
			{
			  /* If the item is NOT marked blank in this line */

			  fprintf(outfptr,"%s <= ",ports[field_cnt].name);
			  fprintf(outfptr,"%s",ports[field_cnt].data_type);
			  fprintf(outfptr,"%s",var);
			  fprintf(outfptr,"%s;\n",ports[field_cnt].data_type);
			}
		      
		      if (strcmp(ports[field_cnt+1].port_type,"out") == 0)
			{
			  /* If its the last input port in this line */
			  
			  fprintf(outfptr,"\nwait for 4 ns;\n\n");
			  fprintf(outfptr,"clk <= '0';\n\n");
			  fprintf(outfptr,"wait for 4 ns;\n\n");
			}
		    } 
		  else
		    {
		      /* If its an output port */
		      
		      if (var[0] != '-')
			{
			  /* If the item is NOT marked blank in this line */

			  fprintf(outfptr,"assert (%s =",ports[field_cnt].name);
			  fprintf(outfptr," %s",ports[field_cnt].data_type);
			  fprintf(outfptr,"%s",var);
			  fprintf(outfptr,"%s)\n",ports[field_cnt].data_type);
			  fprintf(outfptr,"report\n");
			  fprintf(outfptr,"\"Assert %i : < ",assert_cnt);
			  fprintf(outfptr,"%s /= ",ports[field_cnt].name);
			  fprintf(outfptr,"%s >\"",var);
			  
			  /* Vector Count is printed only when outputs are */
			  /* tested and that too only once for each vector */
		      
			  if(printed_vec_no == 0)
			    {
			      fprintf(outfptr," -- \tVector No: %i",vec_cnt);
			      vec_cnt++;
			      printed_vec_no = 1;
			    }

			  fprintf(outfptr,"\n");
			  fprintf(outfptr,"severity warning;\n\n");
			  assert_cnt++;
			}
		    }

		  field_cnt++;
		}
	      /* Print ending statements for test vector */
	      
	      fprintf(outfptr,"wait for 1 ns;\n\n");
	    }
	  
	}
      
      line_cnt++;
    }

  fprintf(outfptr,"--------------------------\n\n");
  fprintf(outfptr,"end process;\n\n");
  fprintf(outfptr,"end AA;\n");
}






