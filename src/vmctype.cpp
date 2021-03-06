#include "vmctype.h"

void vmctype::to_json(json& j, const vmctype::HoppingTerm& p) {
	j = json{
			{"spin row", p.spin_row},
			{"strength", p.strength},
			{"distance", p.distance},
			{"origins", p.origins},
			{"neighbor index", p.neighbor_index},
			{"phases", p.phases},
	};
}

void vmctype::from_json(const json& j, vmctype::HoppingTerm& p) {
	j.at("spin row").get_to(p.spin_row);
	j.at("strength").get_to(p.strength);
	j.at("distance").get_to(p.distance);
	j.at("origins").get_to<std::vector<int>>(p.origins);
	j.at("neighbor index").get_to<std::vector<int>>(p.neighbor_index);
	j.at("phases").get_to<std::vector<double>>(p.phases);
}

void vmctype::to_json(json& j, const vmctype::QuadrupoleOrder& p) {
	j = json{
		{"u_r_theta_phi", p.unit_cell_u_polar},
		{"v_r_theta_phi", p.unit_cell_v_polar},
		{"Qthetau", p.Q_t_u},
		{"Qphiu", p.Q_p_u},
		{"Qthetav", p.Q_t_v},
		{"Qphiv", p.Q_p_v},
	};
}

void vmctype::from_json(const json& j, vmctype::QuadrupoleOrder& p) {
	j.at("u_r_theta_phi").get_to<std::vector<vec3<double>>>(p.unit_cell_u_polar);
	j.at("v_r_theta_phi").get_to<std::vector<vec3<double>>>(p.unit_cell_v_polar);
	j.at("Qthetau").get_to<vec3<double>>(p.Q_t_u);
	j.at("Qphiu").get_to<vec3<double>>(p.Q_p_u);
	j.at("Qthetav").get_to<vec3<double>>(p.Q_t_v);
	j.at("Qphiv").get_to<vec3<double>>(p.Q_p_v);
}

void vmctype::to_json(json& j, const vmctype::JastrowFactorOptions& p) {
	j = json{
		{"isotropic", p.isotropic},
		{"distance_max", p.distance_max},
		{"values", p.values}
	};
}

void vmctype::from_json(const json& j, vmctype::JastrowFactorOptions& p) {
	j.at("isotropic").get_to(p.isotropic);
	j.at("distance_max").get_to(p.distance_max);
	j.at("values").get_to<std::vector<double>>(p.values);
}

void vmctype::to_json(json& j, const vmctype::BilinearOptions& p) {
	j = json{
		{"type", p.interaction_type},
		{"coupling", p.coupling},
		{"neighbor_index", p.neighbor_index}
	};
}

void vmctype::from_json(const json& j, vmctype::BilinearOptions& p) {
	j.at("type").get_to(p.interaction_type);
	j.at("coupling").get_to(p.coupling);
	j.at("neighbor_index").get_to(p.neighbor_index);
}

void vmctype::to_json(json& j, const vmctype::TrilinearOptions& p) {
	j = json{
		{"type", p.interaction_type},
		{"hermitian", p.hermitian},
		{"coupling_real", p.coupling_real},
		{"coupling_imag", p.coupling_imag},
	};
}

void vmctype::from_json(const json& j, vmctype::TrilinearOptions& p) {
	j.at("type").get_to(p.interaction_type);
	j.at("hermitian").get_to(p.hermitian);
	j.at("coupling_real").get_to(p.coupling_real);
	j.at("coupling_imag").get_to(p.coupling_imag);
}