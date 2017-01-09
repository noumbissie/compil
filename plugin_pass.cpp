#include <gcc-plugin.h>                                                         
#include <plugin-version.h>                                                     
#include <tree.h>
#include <basic-block.h>
#include <gimple.h>
#include <tree-pass.h>
#include <context.h>
#include <function.h>
#include <gimple-iterator.h> 

int plugin_is_GPL_compatible;

const pass_data my_pass_data =
{ 
	GIMPLE_PASS, /* type */ 
	"pass", /* name */
	OPTGROUP_NONE, /* optinfo_flags */
	TV_OPTIMIZE, /* tv_id */
	0, /* properties_required */
	0, /* properties_provided */
	0, /* properties_destroyed */
	0, /* todo_flags_start */
	0, /* todo_flags_finish */
};              


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
	        return true;
        }


          /* Execute function */
        unsigned int execute (function *fun)
        {
	
	       printf("in function :%s\n",flname);

			return 0;
        }


}; 





int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
	printf("hello\n");
	
	my_pass p(g);

	/* Fill info on my pass 
	 (insertion after the pass building the CFG) */
	struct register_pass_info my_pass_info;


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
