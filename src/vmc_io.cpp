#include "vmc_io.h"

//------------------------------------------
//	Forward Declarations
//------------------------------------------


//------------------------------------------
//	General Helper Functions
//------------------------------------------



bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
	struct _stat info;
	if (_stat(path.c_str(), &info) != 0)
	{
		return false;
	}
	return (info.st_mode & _S_IFDIR) != 0;
#else 
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool makePath(const std::string& path)
{
#if defined(_WIN32)
	int ret = _mkdir(path.c_str());
#else
	mode_t mode = 0755;
	int ret = mkdir(path.c_str(), mode);
#endif
	if (ret == 0)
		return true;

	switch (errno)
	{
	case ENOENT:
		// parent didn't exist, try to create it
	{
		int pos = path.find_last_of('/');
		if (pos == std::string::npos)
#if defined(_WIN32)
			pos = path.find_last_of('\\');
		if (pos == std::string::npos)
#endif
			return false;
		if (!makePath(path.substr(0, pos)))
			return false;
	}
	// now, try to create again
#if defined(_WIN32)
	return 0 == _mkdir(path.c_str());
#else 
	return 0 == mkdir(path.c_str(), mode);
#endif

	case EEXIST:
		// done!
		return isDirExist(path);

	default:
		return false;
	}
}



lattice_options read_json_lattice(json j) {
	lattice_options lat_opt;
	lat_opt.type = j["lattice"]["type"].get<std::string>();
	lat_opt.dimension = j["lattice"]["dimension"];
	lat_opt.L = j["lattice"]["L"].get<std::vector<int>>();
	lat_opt.pbc = j["lattice"]["pbc"].get<std::vector<int>>();

	lat_opt.Lx = lat_opt.L[0];
	lat_opt.Ly = lat_opt.L[1];
	lat_opt.Lz = lat_opt.L[2];
	return lat_opt;
}

lattice_options read_json_lattice(std::string infile_name) {
	std::ifstream i(infile_name);
	json j;
	i >> j;
	return read_json_lattice(j);
}

mean_field_options read_json_wavefunction(json j) {
	mean_field_options wf_opt;
	wf_opt.lattice_type = j["wavefunction"]["lattice type"].get<std::string>();
	wf_opt.wf_type = j["wavefunction"]["wavefunction type"].get<std::string>();

	if (wf_opt.wf_type == "meanfield") {
		if (j["wavefunction"].contains("basis")) {
			wf_opt.basis = j["wavefunction"]["basis"].get<std::vector<vec3<int>>>();
			wf_opt.inequivalent_sites = j["wavefunction"]["inequivalent sites"];
		}
		wf_opt.spin = j["wavefunction"]["spin"];
		wf_opt.field = j["wavefunction"]["field"];
		wf_opt.num_spin_orbit = j["wavefunction"]["num spin-orbit"];
		wf_opt.match_lattice_pbc = j["wavefunction"]["match lattice pbc"];
		wf_opt.su3_symmetry = j["wavefunction"]["su3_symmetry"];
		wf_opt.hopping_list = j["wavefunction"]["hopping terms"].get<std::vector<HoppingTerm>>();
		if (j["wavefunction"].contains("directors")) {
			wf_opt.directors = j["wavefunction"]["directors"].get<QuadrupoleOrder>();
		}
		if (j["wavefunction"].contains("jastrow")) {
			wf_opt.jastrow_flag = true;
			if (j["wavefunction"]["jastrow"].contains("density")) {
				wf_opt.jastrow.density_flag = true;
				wf_opt.jastrow.density_coupling = j["wavefunction"]["jastrow"]["density"]["strength"];
			}
			if (j["wavefunction"]["jastrow"].contains("sz")) {
				wf_opt.jastrow.sz = j["wavefunction"]["jastrow"]["sz"].get<JastrowFactorOptions>();
			}
			if (j["wavefunction"]["jastrow"].contains("sz2")) {
				wf_opt.jastrow.sz2 = j["wavefunction"]["jastrow"]["sz2"].get<JastrowFactorOptions>();
			}
		}
	}
	else {
		throw;
	}
	return wf_opt;
}

mean_field_options read_json_wavefunction(std::string infile_name) {
	std::ifstream i(infile_name);
	json j;
	i >> j;
	return read_json_wavefunction(j);
}

model_options read_json_model(json j) {
	model_options model_opt;
	model_opt.model_type = j["model"]["type"].get<std::string>(); // either blbq or su3
	if (j["model"].contains("single_ion")) {
		model_opt.single_ion = j["model"]["single_ion"];
	}
	if (j["model"].contains("bilinear")) {
		model_opt.bilinear_terms = j["model"]["bilinear"].get<std::vector<BilinearOptions>>();
	}
	if (j["model"].contains("trilinear")) {
		model_opt.ring3_terms = j["model"]["trilinear"].get<std::vector<TrilinearOptions>>();
	}
	return model_opt;
}

model_options read_json_model(std::string infile_name) {
	std::ifstream i(infile_name);
	json j;
	i >> j;
	return read_json_model(j);
}

vmc_options read_json_vmc(json j) {
	vmc_options vmc_opt;
	if (j["vmc"].contains("optimization")) {
		vmc_opt.optimization = j["vmc"]["optimization"].get<bool>();
		if (vmc_opt.optimization) {
			vmc_opt.sr.bins = j["vmc"]["SR"]["bins"].get<int>();
			vmc_opt.sr.timestep = j["vmc"]["SR"]["timestep"].get<double>();
		}
	}
	else {
		vmc_opt.optimization = false;
	}
	if (j["vmc"].contains("su3")) {
		vmc_opt.su3 = j["vmc"]["su3"].get<bool>();
	}
	else {
		vmc_opt.su3 = true;
	}
	
	vmc_opt.num_measures = j["vmc"]["measures"].get<int>();
	vmc_opt.steps_per_measure = j["vmc"]["steps"].get<int>();
	vmc_opt.throwaway_measures = j["vmc"]["throwaway"].get<int>();

	return vmc_opt;
}

vmc_options read_json_vmc(std::string infile_name) {
	std::ifstream i(infile_name);
	json j;
	i >> j;
	return read_json_vmc(j);
}

void read_json_full_input(lattice_options* lat, mean_field_options* wf, model_options* H, vmc_options* vmc, std::string infile_name) {
	std::ifstream i(infile_name);
	json j;
	i >> j;
	*lat = read_json_lattice(j);
	*wf = read_json_wavefunction(j);
	*H = read_json_model(j);
	*vmc = read_json_vmc(j);
}


std::string lattice_options::to_string() {
		std::stringstream ss;
		ss << "Lattice=" << type << "\n"
			<< "dimension=" << dimension << "\n"
			<< "Lx=" << Lx << "\n" << "Ly=" << Ly << "\n" << "Lz=" << Lz << "\n";
		return ss.str();
}


std::string mean_field_options::to_string() {
		std::stringstream ss;
		ss << "Lattice=" << lattice_type << "\n"
			<< "Inequivalent sites=" << inequivalent_sites << "\n";
			//<< "tz magnitudes: " << vec2str(tz_list) << "\n"
			//<< "tz phases (degrees): " << vec2str(tz_phase_list) << "\n"
			//<< "txy magnitudes: " << vec2str(txy_list) << "\n"
			//<< "tz phases (degrees): " << vec2str(txy_phase_list) << "\n";
		return ss.str();
}