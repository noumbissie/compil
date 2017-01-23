#include <gcc-plugin.h>                                                          
#include <plugin-version.h>                                                      
#include <tree.h>                                                                
#include <basic-block.h>                                                         
#include <gimple.h>                                                              
#include <tree-pass.h>                                                           
#include <context.h>                                                             
#include <function.h>                                                            
#include <gimple-iterator.h>                                                     
#include <c-family/c-common.h>



int plugin_is_GPL_compatible;


const pass_data my_pass_data =
{ 
	GIMPLE_PASS, /* type */ 
	"my_pass", /* name */
	OPTGROUP_NONE, /* optinfo_flags */
	TV_OPTIMIZE, /* tv_id */
	0, /* properties_required */
	0, /* properties_provided */
	0, /* properties_destroyed */
	0, /* todo_flags_start */
	0, /* todo_flags_finish */
};              



/* Enum to represent the collective operations */

enum mpi_collective_code {
#define DEFMPICOLLECTIVES( CODE, NAME ) CODE,
#include "MPI_collectives.def"
        LAST_AND_UNUSED_MPI_COLLECTIVE_CODE
#undef DEFMPICOLLECTIVES
} ;

/* Name of each MPI collective operations */

#define DEFMPICOLLECTIVES( CODE, NAME ) NAME,
const char *const mpi_collective_name[] = {
#include "MPI_collectives.def"
} ;
#undef DEFMPICOLLECTIVES



/****************************************************************************************************/
/****************************************************************************************************/
/*************************                PLUGIN GRAPHVIZ                   *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/* Build a filename (as a string) based on function name */
static char * cfgviz_generate_filename( function * fun, const char * suffix )
{
	char * target_filename ; 

	target_filename = (char *)xmalloc( 1024 * sizeof( char ) ) ;

	snprintf( target_filename, 1024, "%s_%s_%d_%s.dot", 
			current_function_name(),
			LOCATION_FILE( fun->function_start_locus ),
			LOCATION_LINE( fun->function_start_locus ),
			suffix ) ;

	return target_filename ;
}

/* Dump the graphviz representation of function 'fun' in file 'out' */
static void cfgviz_internal_dump( function * fun, FILE * out, int td ) 
{
	basic_block bb; 

	// Print the header line and open the main graph
	fprintf(out, "Digraph G{\n");

	FOR_ALL_BB_FN(bb,cfun)
	{

		fprintf( out,
				"%d [label=\"BB %d\" shape=ellipse]\n",
				bb->index,
				bb->index
		       ) ;
		edge_iterator eit;
		edge e;

		FOR_EACH_EDGE( e, eit, bb->succs )
		{
			const char *label = "";
			if( e->flags == EDGE_TRUE_VALUE )
				label = "true";
			else if( e->flags == EDGE_FALSE_VALUE )
				label = "false";

			fprintf( out, "%d -> %d [color=green label=\"%s\"]\n",
					bb->index, e->dest->index, label ) ;

		}
	}
	fprintf(out, "}\n");
}

void cfgviz_dump( function * fun, const char * suffix, int td )
{
	char * target_filename ; 
	FILE * out ;

	target_filename = cfgviz_generate_filename( fun, suffix ) ;

	printf( "[GRAPHVIZ] Generating CFG of function %s in file <%s>\n",
			current_function_name(), target_filename ) ;

	out = fopen( target_filename, "w" ) ;

	cfgviz_internal_dump( fun, out, td ) ;

	fclose( out ) ;
	free( target_filename ) ;
}


/****************************************************************************************************/
/****************************************************************************************************/
/*************************            FIN  PLUGIN GRAPHVIZ                 *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/*-------------------------------------------------------------Premiere Partie-------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/



//****************************************************************************************************************************************************//
//*****************************************************Parcours de graphe et recherche de fonctions MPI*********************************************************//
//****************************************************************************************************************************************************//




void search_mpi_funct_and_mark(function *fun)
{
	
	basic_block b;
	gimple_stmt_iterator gsi;
	gimple *stmt;
    int i;
    
	FOR_EACH_BB_FN(b,fun)
	{

	for (gsi = gsi_start_bb (b); !gsi_end_p (gsi); gsi_next (&gsi))
	{
		stmt=gsi_stmt(gsi);
		
		i=0;
		while(i < LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
		{
			
			if (is_gimple_call (stmt))
			{
				tree t ;
				const char * callee_name ;
				t = gimple_call_fndecl( stmt ) ;
				enum mpi_collective_code val;
				
				callee_name = IDENTIFIER_POINTER(DECL_NAME(t)) ;		     
				if ( strcmp( callee_name, mpi_collective_name[i]) == 0 )
                      if(b->aux == (void *) LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
			          {   val=(enum mpi_collective_code)i;
				          b->aux = (void *)val ;
			          }
					
			
			} 
			i++;           
        }
    }
 	}

}


//****************************************************************************************************************************************************//
//******************************************************************fonctions intermediaires *********************************************************//
//****************************************************************************************************************************************************//

void bitmap_postdom_front (bitmap_head *Pfront, function *fun)
{
    edge p;
	edge_iterator ei;
	basic_block b,runner;

	FOR_EACH_BB_FN (b, fun)
	{

		if (EDGE_COUNT (b->succs) >= 2)
		{
			FOR_EACH_EDGE (p, ei, b->succs)
			{
				runner = p->dest;
				basic_block pdomsb; 

				if (runner == EXIT_BLOCK_PTR_FOR_FN (fun))
					continue;
                
			    pdomsb = get_immediate_dominator (CDI_POST_DOMINATORS, b);
                 
				while (runner != pdomsb)
				{
					if (!bitmap_set_bit (&Pfront[runner->index], b->index))
						break;

					runner = get_immediate_dominator (CDI_POST_DOMINATORS, runner);
				}
			}
		}
	}





}






void bitmap_set_postdom_front(bitmap_head node_set,bitmap_head *pdf, bitmap_head *pdf_set,function *fun)
{

    basic_block b1;
    
    
    
	/* Create the union of each PDF */
	
	FOR_ALL_BB_FN( b1, cfun )
	{
		if ( bitmap_bit_p( &node_set, b1->index ) )
		{
			bitmap_ior_into( pdf_set, &pdf[b1->index] ) ;
		}
	}

	
	/* Remove the nodes that have no other PDF remaining */
	bitmap_print( stdout, pdf_set, "Partial PDF of set: ", "\n" ) ;
	
	FOR_ALL_BB_FN( b1, cfun )
	{
		if ( bitmap_bit_p( pdf_set, b1->index ) )
		{
			bool found = false ;
	        basic_block b2;
			FOR_ALL_BB_FN( b2, cfun )
			{
				if ( ! bitmap_bit_p( &node_set, b2->index ) && bitmap_bit_p( &pdf[b2->index], b1->index) )
				{
					found = true ;
				}
			}

				if ( found == false )
				{
					bitmap_clear_bit( pdf_set, b1->index ) ;
				}
		}
	}
	
      bitmap_print( stdout, pdf_set, "Final PDF of set: ", "\n" ) ;
      
}







void iterated_post_dom(bitmap_head pdf_node_set,bitmap_head *pdf, bitmap_head *ipdf_set,function * fun)
{
	basic_block b1, b2;
	bitmap_head bitmap_tmp, bitmap_test;
	bitmap_initialize (&bitmap_tmp, &bitmap_default_obstack);
	bitmap_initialize (&bitmap_test, &bitmap_default_obstack);
	
	//bitmap test iteration courante
    //bitmap tmp iteration avant

	FOR_ALL_BB_FN( b1,fun )
	{
		if ( bitmap_bit_p( &pdf_node_set, b1->index ) )
		{
			
			bitmap_copy(&bitmap_test, &pdf_node_set);
			while(!bitmap_equal_p(&bitmap_tmp, &bitmap_test) )
			{
				bitmap_copy(&bitmap_tmp, &bitmap_test);
				FOR_ALL_BB_FN( b2, fun )
				{
					if(bitmap_bit_p(&pdf[b1->index], b2->index))
					{
						bitmap_set_bit(&bitmap_test, b2->index);
					}
					
				}
			}	
			bitmap_copy(&bitmap_test, ipdf_set);
			bitmap_ior(ipdf_set, &bitmap_test, &bitmap_tmp);	
		}
	}
	
		
}





void print_place(int plc,const char * name)
{
	printf("\n\n\n");
	printf("/****************************************************************************************************/\n");
	printf("/****************************************************************************************************/\n");
	printf("/*************************                       %s Partie %d                       ********************/\n",name, plc);
	printf("/****************************************************************************************************/\n");
	printf("/****************************************************************************************************/\n");
	printf("\n\n\n");

 
}





enum mpi_collective_code mpi_call( gimple *stmt,int bb_index){
	
    enum mpi_collective_code returned_code ;

	returned_code = LAST_AND_UNUSED_MPI_COLLECTIVE_CODE ;

	if (is_gimple_call (stmt))
	{
		tree t ;
		const char * callee_name ;
		int i ;
		bool found = false ;

		t = gimple_call_fndecl( stmt ) ;
		callee_name = IDENTIFIER_POINTER(DECL_NAME(t)) ;

		i = 0 ;
		while ( !found && i < LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
		{
			if ( strncmp( callee_name, mpi_collective_name[i], strlen(
							mpi_collective_name[i] ) ) == 0 )
			{
				found = true ;
				returned_code = (enum mpi_collective_code) i ;
			}
			i++ ;
		} 

	}

	return returned_code ;

}



int get_nb_mpi_calls_in_bb(basic_block b)
{
	
	gimple_stmt_iterator gsi;
		int nb_mpi_coll = 0 ;

		for (gsi = gsi_start_bb (b); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gimple *stmt = gsi_stmt (gsi); 

			enum mpi_collective_code c ;
			c = mpi_call( stmt, b->index ) ;

			if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
			{
				nb_mpi_coll++ ;
			}
		}

		printf( " == BB %d == contains %d MPI collective(s)\n", 
				b->index, nb_mpi_coll ) ;

		return nb_mpi_coll ;
}





bool check_multiple_mpi_calls_per_bb(function *fun)
{
	basic_block bb;
	bool is_multiple_mpi_coll = false;
	int nb_mpi_coll_in_bb = 0;
	FOR_EACH_BB_FN (bb, fun)
	{
		nb_mpi_coll_in_bb = get_nb_mpi_calls_in_bb(bb);	
		if(nb_mpi_coll_in_bb > 1){
			is_multiple_mpi_coll = true;
		}
	}

	return is_multiple_mpi_coll;
}


void split_multiple_mpi_calls( function * fun )
{
	basic_block bb; 

	FOR_EACH_BB_FN (bb, fun)
	{
		int n =get_nb_mpi_calls_in_bb( bb ) ;

		if ( n > 1 ) 
		{
			gimple_stmt_iterator gsi;
			for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
			{
				gimple *stmt = gsi_stmt (gsi); 
				enum mpi_collective_code c ;

				c = mpi_call( stmt, bb->index ) ;

				if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
				{
					split_block( bb, stmt ) ;
				}

			}
		}
	}
}




void clean_aux_field( function * fun, long val )
{
	basic_block bb; 
	FOR_ALL_BB_FN (bb, fun)
	{
		bb->aux = (void *)val ;
	}

}


void mpi_in_block(function *fun ){
	
	
	bool do_splitting = false;
	
	search_mpi_funct_and_mark(fun);
	do_splitting = check_multiple_mpi_calls_per_bb(fun);

	if(do_splitting)
	{
		split_multiple_mpi_calls(fun);
	}

	clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
	search_mpi_funct_and_mark(fun);	
	
	
}




//****************************************************************************************************************************************************//
//*****************************************************Affichage D'un Warning à L'utilisateur*********************************************************//
//****************************************************************************************************************************************************//


void detect_issue_warnings(bitmap_head setof_ipdf[],function *fun)
{
	
	basic_block b;
	gimple_stmt_iterator gsi;
	gimple *stmt;
	int k;

    for(k=0; k<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; k++)
	{
		FOR_EACH_BB_FN( b, fun )
		{
			if(bitmap_bit_p(&setof_ipdf[k],b->index))
			{
				printf("-------------WARNING !!! ---------------------- \n");
    
				gsi = gsi_start_bb(b);
				stmt = gsi_stmt(gsi);
			    bitmap_print( stdout, &setof_ipdf[k], "", "\n" ) ;
				printf(" Basic Block  %d  at line %d might cause an issue \n", b->index, gimple_lineno(stmt)); 
				
			}
			
		}
	 }

}





void postdom(function *fun){
	
	calculate_dominance_info (CDI_POST_DOMINATORS);
	edge p;
	edge_iterator ei;
	basic_block b;

	int cpt = 0;
	int i;	
	int number_bbs = last_basic_block_for_fn (fun);
	int isPDF[number_bbs][number_bbs+1];
	memset (&isPDF, 0, number_bbs * (number_bbs+1) * sizeof(int)); 

	FOR_EACH_BB_FN(b, fun)
	{ 

		if (EDGE_COUNT (b->succs) >= 2)
		{
			FOR_EACH_EDGE (p, ei, b->succs)
			{
				basic_block runner = p->dest;
				basic_block pdomsb; 

				if (runner == EXIT_BLOCK_PTR_FOR_FN (fun))
					continue;
				pdomsb = get_immediate_dominator (CDI_POST_DOMINATORS, b);
				while (runner != pdomsb)
				{
					if (0 == isPDF[runner->index][b->index+1])
					{
						isPDF[runner->index][b->index+1] = 1;
						isPDF[runner->index][0]++;
						cpt++;
						printf("%d\n", cpt);
					}
					else
					{
						break;
					}
					runner = get_immediate_dominator (CDI_POST_DOMINATORS, runner);
				}
			}
		}
	}
	for(i=0; i<number_bbs; i++)
	{
		if(0 == isPDF[i][0])
		{
			printf("Basic block %d has empty PDF\n", i); fflush(stdout);
		}
		else
		{
			printf("Basic block %d has %d basic blocks in its PDF:\n", i, isPDF[i][0]); fflush(stdout);
			int j;
			for(j=0; j<number_bbs; j++)
			{
				if(0 != isPDF[i][j+1])
				{
					printf("------- Basic Block %d\n", j); fflush(stdout);
				}
			}
		}
	}
	
	

}








//*********************************************************************************************************************************************************************//
//******************************************************Construction de des frontieres de postdominance itérée********************************************************//
//*******************************************************************************************************************************************************************//



void construct_pdf_iterated(function *fun,bitmap_head ipdf_set[]){

	bitmap_head *Pfront;
	basic_block bb;
	Pfront= XNEWVEC (bitmap_head, last_basic_block_for_fn (fun));

	FOR_ALL_BB_FN (bb, cfun)
	{
		bitmap_initialize (&Pfront[bb->index], &bitmap_default_obstack);
	}

	bitmap_postdom_front (Pfront, fun);
	/*
	FOR_ALL_BB_FN (bb, cfun)
	{
		printf( "PDF Node %d: ", bb->index ) ;
		bitmap_print( stdout, &Pfront[bb->index], "", "\n" ) ;
	}*/
	
    //bitmap_head ipdf_set[LAST_AND_UNUSED_MPI_COLLECTIVE_CODE];
    bitmap_head pdf_set;
    int i;

   for(i=0;i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE;i++){

	/* Compute the set regrouping nodes with MPI calls */
	bitmap_head mpi_set ;
	bitmap_initialize( &mpi_set,  &bitmap_default_obstack);
    
    
	FOR_ALL_BB_FN (bb, cfun)
	{ // est ce que le numero de la collective courante est égale au numero de la collective testée
		if ( bb->aux == (void *)i) 
		{
			bitmap_set_bit( &mpi_set, bb->index ) ;
		}
	}
	
	//bitmap_head ipdf_set;
	bitmap_initialize (&pdf_set, &bitmap_default_obstack);

    bitmap_set_postdom_front(mpi_set,Pfront, &pdf_set, fun);
	
   
	/* if the PDF of the set it not empty => issues! */
	if ( bitmap_count_bits( &pdf_set ) > 0 )
	{
		printf( "ISSUES WITH MPI CALLS\n" ) ;
	} 
	else
	{
		printf( "NO PROBLEM WITH THIS MPI CALLS ! \n" ) ;
	}
	//bitmap_head ipdf_set;
	bitmap_initialize (&ipdf_set[i], &bitmap_default_obstack);

	iterated_post_dom(pdf_set, Pfront, &ipdf_set[i], fun);

    //detect_issue_warnings (ipdf_set[i], fun);

}
 detect_issue_warnings (ipdf_set, fun);
	  
}



void checking_and_print(function *fun,bitmap_head ipdf_set[]){
		
	basic_block bb;
	gimple_stmt_iterator gsi;
	gimple *stmt;

	int i;

	int cpt = 0;
   
    
	for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++)
	{
		cpt = bitmap_count_bits(&ipdf_set[i]);
			
		if(cpt > 0 || MPI_FINALIZE == i )
		{
            cpt = bitmap_count_bits(&ipdf_set[i]);
            
			FOR_ALL_BB_FN (bb, fun)
			{
				if ( bb->aux == (void *) i) 
				{
					/* Iterate on gimple statements in the current basic block */
					for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
					{
						stmt = gsi_stmt(gsi);

						enum mpi_collective_code val;
						val = mpi_call(stmt, bb->index);
						if ( val != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
						{

							char error_message[cpt*32+2]; 
							for(int l=0; l<cpt*32+2; l++) error_message[l] = '\0';
							error_message[0] = '-';
							error_message[1] = '-';
							char tmp_message[32]; 
							for(int l=0; l<32; l++) tmp_message[l] = ' ';
							printf("1-- TMP MESSAGE =      --      ERROR MESSAGE = %s\n",&error_message[0]); fflush(stdout);

							    basic_block bb2;
						        gimple_stmt_iterator gsi2;
						        gimple *stmt2;

							printf("Collective %s - Basic Block %d (line %d)\n", mpi_collective_name[i], bb->index, gimple_lineno(gsi_stmt(gsi_start_bb(bb))));
							FOR_EACH_BB_FN( bb2, fun )
							{
								if(bitmap_bit_p(&ipdf_set[i], bb2->index))
								{	
									for(int l=0; l<32; l++) tmp_message[l] = '\0';
									gsi2 = gsi_start_bb(bb2);
									stmt2 = gsi_stmt(gsi2);
									printf("...... Basic Block  %d  (line %d)\n", bb2->index, gimple_lineno(stmt2));

									sprintf(&tmp_message[0], "Basic Block  %d  (line %d) , ", bb2->index, gimple_lineno(stmt2));
									strncat(&error_message[0], &tmp_message[0], 32); 

								}
							}

							printf("2-- TMP MESSAGE = %s     --      ERROR MESSAGE = %s\n", tmp_message, error_message); fflush(stdout);
							printf(" COLLECTIVE NAME = %s -- length = %d\n", mpi_collective_name[i], strlen(mpi_collective_name[i]));	
							printf(" ERROR MESSAGE = %s -- length = %d\n", &error_message[0], strlen(&error_message[0]));	

							tree MPI_coll_name_tree = fix_string_type( build_string (strlen(mpi_collective_name[i]), mpi_collective_name[i]));	
							tree MPI_coll_name_ptr = build_pointer_type(TREE_TYPE (TREE_TYPE (MPI_coll_name_tree)));
							tree MPI_coll_name_arg = build1 (ADDR_EXPR, MPI_coll_name_ptr, MPI_coll_name_tree);



							tree bb_list_tree = fix_string_type( build_string (cpt*32+2, &error_message[0]));	
							tree bb_list_ptr = build_pointer_type(TREE_TYPE (TREE_TYPE (bb_list_tree)));
							tree bb_list_arg = build1 (ADDR_EXPR, bb_list_ptr, bb_list_tree);


							tree MPI_coll_id_arg = build_int_cst(integer_type_node, i);
							tree MPI_coll_line_arg = build_int_cst(integer_type_node, gimple_lineno(stmt));

							tree function_type_list = build_function_type_list(void_type_node, void_type_node, NULL_TREE);
							tree cc_fn_decl = build_fn_decl("checking_c", function_type_list);


							gimple * cc_call = gimple_build_call ( cc_fn_decl, 4, MPI_coll_id_arg, MPI_coll_name_arg, MPI_coll_line_arg, bb_list_arg);

							gimple_set_location(cc_call, gimple_location(stmt));
							gsi_insert_before( &gsi , cc_call, GSI_NEW_STMT);

							gsi_next (&gsi);
						}
						
					}	
				}
			}
		}

	}

	
}








/* My new pass inheriting from regular gimple pass */
class my_pass : public gimple_opt_pass
{
	public:
		my_pass (gcc::context *ctxt)
			: gimple_opt_pass (my_pass_data, ctxt)
		{}

		/* opt_pass methods: */

		my_pass *clone ()
		{
			return new my_pass(g);
		}


		bool gate (function *fun)
		{
			printf("in function gate pass :\n");
	       
			//search_mpi_funct_and_mark(fun);
			
			
	        return true;
        }


          /* Execute function */
        unsigned int execute (function *fun)
        {
	       printf("in function execute pass :\n");
	       bitmap_head ipdf_set[LAST_AND_UNUSED_MPI_COLLECTIVE_CODE];
	       
	       clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
	       /*************Premiere partie *******************/
	       print_place(1,"DEBUT");
	       
	       mpi_in_block(fun);
	       
	       postdom(fun);
	       //search_mpi_funct_and_mark(fun);
	      
	       construct_pdf_iterated(fun,ipdf_set);
	       
	       print_place(1,"FIN");
	       
	      
	       /*************Fin Premiere partie *******************/
	       
	       /**********debut deuxieme partie ********/
	       
	       print_place(2,"DEBUT");
	       
	       checking_and_print(fun,ipdf_set);
	       
	       print_place(2,"FIN");
	       
	       
	       /****Fin deuxieme partie ***************/
	       
	       clean_aux_field(fun, 0);
	       
	       free_dominance_info( CDI_DOMINATORS ) ;
		   free_dominance_info( CDI_POST_DOMINATORS ) ;


			return 0;
        }


}; 





int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
	struct register_pass_info my_pass_info;

	printf( "plugin_init: Entering...\n" ) ;

	/* First check that the current version of GCC is the right one */

	if(!plugin_default_version_check(version, &gcc_version)) 
		return 1;

	printf( "plugin_init: Check ok...\n" ) ;
	

	/* Declare and build my new pass */
	my_pass p(g);

	/* Fill info on my pass 
	 (insertion after the pass building the CFG) */
	my_pass_info.pass = &p;
	my_pass_info.reference_pass_name = "cfg";
	my_pass_info.ref_pass_instance_number = 0;
	my_pass_info.pos_op = PASS_POS_INSERT_AFTER;

	/* Add my pass to the pass manager */
	register_callback(plugin_info->base_name, 
			PLUGIN_PASS_MANAGER_SETUP, 
			NULL, 
			&my_pass_info);

	printf( "plugin_init: Pass added...\n" ) ;
	
	

	return 0;
}




