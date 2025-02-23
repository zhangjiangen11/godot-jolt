#include "jolt_generic_6dof_joint_impl_3d.hpp"

#include "objects/jolt_body_impl_3d.hpp"
#include "spaces/jolt_space_3d.hpp"

namespace {

constexpr double DEFAULT_LINEAR_LIMIT_SOFTNESS = 0.7;
constexpr double DEFAULT_LINEAR_RESTITUTION = 0.5;
constexpr double DEFAULT_LINEAR_DAMPING = 1.0;

constexpr double DEFAULT_ANGULAR_LIMIT_SOFTNESS = 0.5;
constexpr double DEFAULT_ANGULAR_DAMPING = 1.0;
constexpr double DEFAULT_ANGULAR_RESTITUTION = 0.0;
constexpr double DEFAULT_ANGULAR_FORCE_LIMIT = 0.0;
constexpr double DEFAULT_ANGULAR_ERP = 0.5;

} // namespace

JoltGeneric6DOFJointImpl3D::JoltGeneric6DOFJointImpl3D(
	const JoltJointImpl3D& p_old_joint,
	JoltBodyImpl3D* p_body_a,
	JoltBodyImpl3D* p_body_b,
	const Transform3D& p_local_ref_a,
	const Transform3D& p_local_ref_b
)
	: JoltJointImpl3D(p_old_joint, p_body_a, p_body_b, p_local_ref_a, p_local_ref_b) {
	rebuild();
}

double JoltGeneric6DOFJointImpl3D::get_param(Axis p_axis, Param p_param) const {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_param) {
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_LOWER_LIMIT: {
			return limit_lower[axis_lin];
		}
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_UPPER_LIMIT: {
			return limit_upper[axis_lin];
		}
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_LIMIT_SOFTNESS: {
			return DEFAULT_LINEAR_LIMIT_SOFTNESS;
		}
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_RESTITUTION: {
			return DEFAULT_LINEAR_RESTITUTION;
		}
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_DAMPING: {
			return DEFAULT_LINEAR_DAMPING;
		}
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_MOTOR_TARGET_VELOCITY: {
			return motor_speed[axis_lin];
		}
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_MOTOR_FORCE_LIMIT: {
			return motor_limit[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_STIFFNESS: {
			return spring_stiffness[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_DAMPING: {
			return spring_damping[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_EQUILIBRIUM_POINT: {
			return spring_equilibrium[axis_lin];
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_LOWER_LIMIT: {
			return limit_lower[axis_ang];
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_UPPER_LIMIT: {
			return limit_upper[axis_ang];
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_LIMIT_SOFTNESS: {
			return DEFAULT_ANGULAR_LIMIT_SOFTNESS;
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_DAMPING: {
			return DEFAULT_ANGULAR_DAMPING;
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_RESTITUTION: {
			return DEFAULT_ANGULAR_RESTITUTION;
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_FORCE_LIMIT: {
			return DEFAULT_ANGULAR_FORCE_LIMIT;
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_ERP: {
			return DEFAULT_ANGULAR_ERP;
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_MOTOR_TARGET_VELOCITY: {
			return motor_speed[axis_ang];
		}
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_MOTOR_FORCE_LIMIT: {
			return motor_limit[axis_ang];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_STIFFNESS: {
			return spring_stiffness[axis_ang];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_DAMPING: {
			return spring_damping[axis_ang];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_EQUILIBRIUM_POINT: {
			return spring_equilibrium[axis_ang];
		}
		default: {
			ERR_FAIL_D_REPORT(vformat("Unhandled parameter: '%d'.", p_param));
		}
	}
}

void JoltGeneric6DOFJointImpl3D::set_param(Axis p_axis, Param p_param, double p_value) {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_param) {
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_LOWER_LIMIT: {
			limit_lower[axis_lin] = p_value;
			_limits_changed();
		} break;
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_UPPER_LIMIT: {
			limit_upper[axis_lin] = p_value;
			_limits_changed();
		} break;
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_LIMIT_SOFTNESS: {
			if (!Math::is_equal_approx(p_value, DEFAULT_LINEAR_LIMIT_SOFTNESS)) {
				WARN_PRINT(vformat(
					"Generic 6DOF joint linear limit softness is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_RESTITUTION: {
			if (!Math::is_equal_approx(p_value, DEFAULT_LINEAR_RESTITUTION)) {
				WARN_PRINT(vformat(
					"Generic 6DOF joint linear restitution is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_DAMPING: {
			if (!Math::is_equal_approx(p_value, DEFAULT_LINEAR_DAMPING)) {
				WARN_PRINT(vformat(
					"Generic 6DOF joint linear damping is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_MOTOR_TARGET_VELOCITY: {
			motor_speed[axis_lin] = p_value;
			_motor_speed_changed(axis_lin);
		} break;
		case PhysicsServer3D::G6DOF_JOINT_LINEAR_MOTOR_FORCE_LIMIT: {
			motor_limit[axis_lin] = p_value;
			_motor_limit_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_STIFFNESS: {
			spring_stiffness[axis_lin] = p_value;
			_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_DAMPING: {
			spring_damping[axis_lin] = p_value;
			_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_EQUILIBRIUM_POINT: {
			spring_equilibrium[axis_lin] = p_value;
			_spring_equilibrium_changed(axis_lin);
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_LOWER_LIMIT: {
			limit_lower[axis_ang] = p_value;
			_limits_changed();
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_UPPER_LIMIT: {
			limit_upper[axis_ang] = p_value;
			_limits_changed();
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_LIMIT_SOFTNESS: {
			if (!Math::is_equal_approx(p_value, DEFAULT_ANGULAR_LIMIT_SOFTNESS)) {
				WARN_PRINT(vformat(
					"Generic 6DOF joint angular limit softness is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_DAMPING: {
			if (!Math::is_equal_approx(p_value, DEFAULT_ANGULAR_DAMPING)) {
				WARN_PRINT(vformat(
					"Generic 6DOF joint angular damping is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_RESTITUTION: {
			if (!Math::is_equal_approx(p_value, DEFAULT_ANGULAR_RESTITUTION)) {
				WARN_PRINT(vformat(
					"Generic 6DOF joint angular restitution is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_FORCE_LIMIT: {
			if (!Math::is_equal_approx(p_value, DEFAULT_ANGULAR_FORCE_LIMIT)) {
				WARN_PRINT(vformat(
					"Generic 6DOF angular force limit is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_ERP: {
			if (!Math::is_equal_approx(p_value, DEFAULT_ANGULAR_ERP)) {
				WARN_PRINT(vformat(
					"Generic 6DOF angular ERP is not supported by Godot Jolt. "
					"Any such value will be ignored. "
					"This joint connects %s.",
					_bodies_to_string()
				));
			}
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_MOTOR_TARGET_VELOCITY: {
			motor_speed[axis_ang] = p_value;
			_motor_speed_changed(axis_ang);
		} break;
		case PhysicsServer3D::G6DOF_JOINT_ANGULAR_MOTOR_FORCE_LIMIT: {
			motor_limit[axis_ang] = p_value;
			_motor_limit_changed(axis_ang);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_STIFFNESS: {
			spring_stiffness[axis_ang] = p_value;
			_spring_parameters_changed(axis_ang);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_DAMPING: {
			spring_damping[axis_ang] = p_value;
			_spring_parameters_changed(axis_ang);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_EQUILIBRIUM_POINT: {
			spring_equilibrium[axis_ang] = p_value;
			_spring_equilibrium_changed(axis_ang);
		} break;
		default: {
			ERR_FAIL_REPORT(vformat("Unhandled parameter: '%d'.", p_param));
		} break;
	}
}

bool JoltGeneric6DOFJointImpl3D::get_flag(Axis p_axis, Flag p_flag) const {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_flag) {
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_LINEAR_LIMIT: {
			return limit_enabled[axis_lin];
		}
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_ANGULAR_LIMIT: {
			return limit_enabled[axis_ang];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_ANGULAR_SPRING: {
			return spring_enabled[axis_ang];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_LINEAR_SPRING: {
			return spring_enabled[axis_lin];
		}
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_MOTOR: {
			return motor_enabled[axis_ang];
		}
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_LINEAR_MOTOR: {
			return motor_enabled[axis_lin];
		}
		default: {
			ERR_FAIL_D_REPORT(vformat("Unhandled flag: '%d'.", p_flag));
		}
	}
}

void JoltGeneric6DOFJointImpl3D::set_flag(Axis p_axis, Flag p_flag, bool p_enabled) {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_flag) {
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_LINEAR_LIMIT: {
			limit_enabled[axis_lin] = p_enabled;
			_limits_changed();
		} break;
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_ANGULAR_LIMIT: {
			limit_enabled[axis_ang] = p_enabled;
			_limits_changed();
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_ANGULAR_SPRING: {
			spring_enabled[axis_ang] = p_enabled;
			_spring_state_changed(axis_ang);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_LINEAR_SPRING: {
			spring_enabled[axis_lin] = p_enabled;
			_spring_state_changed(axis_lin);
		} break;
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_MOTOR: {
			motor_enabled[axis_ang] = p_enabled;
			_motor_state_changed(axis_ang);
		} break;
		case PhysicsServer3D::G6DOF_JOINT_FLAG_ENABLE_LINEAR_MOTOR: {
			motor_enabled[axis_lin] = p_enabled;
			_motor_state_changed(axis_lin);
		} break;
		default: {
			ERR_FAIL_REPORT(vformat("Unhandled flag: '%d'.", p_flag));
		} break;
	}
}

double JoltGeneric6DOFJointImpl3D::get_jolt_param(Axis p_axis, JoltParam p_param) const {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_param) {
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_FREQUENCY: {
			return spring_frequency[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_MAX_FORCE: {
			return spring_limit[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_LIMIT_SPRING_FREQUENCY: {
			return limit_spring_frequency[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_LIMIT_SPRING_DAMPING: {
			return limit_spring_damping[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_FREQUENCY: {
			return spring_frequency[axis_ang];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_MAX_TORQUE: {
			return spring_limit[axis_ang];
		}
		default: {
			ERR_FAIL_D_REPORT(vformat("Unhandled parameter: '%d'.", p_param));
		}
	}
}

void JoltGeneric6DOFJointImpl3D::set_jolt_param(Axis p_axis, JoltParam p_param, double p_value) {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_param) {
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_FREQUENCY: {
			spring_frequency[axis_lin] = p_value;
			_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_SPRING_MAX_FORCE: {
			spring_limit[axis_lin] = p_value;
			_spring_limit_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_LIMIT_SPRING_FREQUENCY: {
			limit_spring_frequency[axis_lin] = p_value;
			_limit_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_LINEAR_LIMIT_SPRING_DAMPING: {
			limit_spring_damping[axis_lin] = p_value;
			_limit_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_FREQUENCY: {
			spring_frequency[axis_ang] = p_value;
			_spring_parameters_changed(axis_ang);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_ANGULAR_SPRING_MAX_TORQUE: {
			spring_limit[axis_ang] = p_value;
			_spring_limit_changed(axis_ang);
		} break;
		default: {
			ERR_FAIL_REPORT(vformat("Unhandled parameter: '%d'.", p_param));
		} break;
	}
}

bool JoltGeneric6DOFJointImpl3D::get_jolt_flag(Axis p_axis, JoltFlag p_flag) const {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_flag) {
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_LINEAR_LIMIT_SPRING: {
			return limit_spring_enabled[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_LINEAR_SPRING_FREQUENCY: {
			return spring_use_frequency[axis_lin];
		}
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_ANGULAR_SPRING_FREQUENCY: {
			return spring_use_frequency[axis_ang];
		}
		default: {
			ERR_FAIL_D_REPORT(vformat("Unhandled flag: '%d'.", p_flag));
		}
	}
}

void JoltGeneric6DOFJointImpl3D::set_jolt_flag(Axis p_axis, JoltFlag p_flag, bool p_enabled) {
	const int32_t axis_lin = AXES_LINEAR + (int32_t)p_axis;
	const int32_t axis_ang = AXES_ANGULAR + (int32_t)p_axis;

	switch ((int32_t)p_flag) {
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_LINEAR_LIMIT_SPRING: {
			limit_spring_enabled[axis_lin] = p_enabled;
			_limit_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_LINEAR_SPRING_FREQUENCY: {
			spring_use_frequency[axis_lin] = p_enabled;
			_spring_parameters_changed(axis_lin);
		} break;
		case JoltPhysicsServer3DExtension::G6DOF_JOINT_FLAG_ENABLE_ANGULAR_SPRING_FREQUENCY: {
			spring_use_frequency[axis_ang] = p_enabled;
			_spring_parameters_changed(axis_ang);
		} break;
		default: {
			ERR_FAIL_REPORT(vformat("Unhandled flag: '%d'.", p_flag));
		} break;
	}
}

float JoltGeneric6DOFJointImpl3D::get_applied_force() const {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	ERR_FAIL_NULL_D(constraint);

	JoltSpace3D* space = get_space();
	ERR_FAIL_NULL_D(space);

	const float last_step = space->get_last_step();
	QUIET_FAIL_COND_D(last_step == 0.0f);

	const JPH::Vec3 lambda = constraint->GetTotalLambdaPosition() +
		constraint->GetTotalLambdaMotorTranslation();
	return lambda.Length() / last_step;
}

float JoltGeneric6DOFJointImpl3D::get_applied_torque() const {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	ERR_FAIL_NULL_D(constraint);

	JoltSpace3D* space = get_space();
	ERR_FAIL_NULL_D(space);

	const float last_step = space->get_last_step();
	QUIET_FAIL_COND_D(last_step == 0.0f);

	const JPH::Vec3 total_lambda = constraint->GetTotalLambdaRotation() +
		constraint->GetTotalLambdaMotorRotation();

	return total_lambda.Length() / last_step;
}

void JoltGeneric6DOFJointImpl3D::rebuild() {
	destroy();

	JoltSpace3D* space = get_space();

	if (space == nullptr) {
		return;
	}

	const JPH::BodyID body_ids[2] = {
		body_a != nullptr ? body_a->get_jolt_id() : JPH::BodyID(),
		body_b != nullptr ? body_b->get_jolt_id() : JPH::BodyID()};

	const JoltWritableBodies3D jolt_bodies = space->write_bodies(body_ids, count_of(body_ids));

	auto* jolt_body_a = static_cast<JPH::Body*>(jolt_bodies[0]);
	auto* jolt_body_b = static_cast<JPH::Body*>(jolt_bodies[1]);

	ERR_FAIL_COND(jolt_body_a == nullptr && jolt_body_b == nullptr);

	Transform3D shifted_ref_a;
	Transform3D shifted_ref_b;

	_shift_reference_frames(Vector3(), Vector3(), shifted_ref_a, shifted_ref_b);

	jolt_ref = _build_6dof(jolt_body_a, jolt_body_b, shifted_ref_a, shifted_ref_b);

	space->add_joint(this);

	_update_enabled();
	_update_iterations();

	_update_limit_spring_parameters(AXIS_LINEAR_X);
	_update_limit_spring_parameters(AXIS_LINEAR_Y);
	_update_limit_spring_parameters(AXIS_LINEAR_Z);

	for (int32_t axis = 0; axis < AXIS_COUNT; ++axis) {
		_update_motor_state(axis);
		_update_motor_velocity(axis);
		_update_motor_limit(axis);
		_update_spring_parameters(axis);
		_update_spring_equilibrium(axis);
	}
}

JPH::Constraint* JoltGeneric6DOFJointImpl3D::_build_6dof(
	JPH::Body* p_jolt_body_a,
	JPH::Body* p_jolt_body_b,
	const Transform3D& p_shifted_ref_a,
	const Transform3D& p_shifted_ref_b
) const {
	JPH::SixDOFConstraintSettings constraint_settings;

	for (int32_t axis = 0; axis < AXIS_COUNT; ++axis) {
		double lower = limit_lower[axis];
		double upper = limit_upper[axis];

		if (axis >= AXIS_ANGULAR_X && axis <= AXIS_ANGULAR_Z) {
			const double temp = lower;
			lower = -upper;
			upper = -temp;
		}

		if (!limit_enabled[axis] || lower > upper) {
			constraint_settings.MakeFreeAxis((JoltAxis)axis);
		} else {
			constraint_settings.SetLimitedAxis((JoltAxis)axis, (float)lower, (float)upper);
		}
	}

	constraint_settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
	constraint_settings.mPosition1 = to_jolt_r(p_shifted_ref_a.origin);
	constraint_settings.mAxisX1 = to_jolt(p_shifted_ref_a.basis.get_column(Vector3::AXIS_X));
	constraint_settings.mAxisY1 = to_jolt(p_shifted_ref_a.basis.get_column(Vector3::AXIS_Y));
	constraint_settings.mPosition2 = to_jolt_r(p_shifted_ref_b.origin);
	constraint_settings.mAxisX2 = to_jolt(p_shifted_ref_b.basis.get_column(Vector3::AXIS_X));
	constraint_settings.mAxisY2 = to_jolt(p_shifted_ref_b.basis.get_column(Vector3::AXIS_Y));
	constraint_settings.mSwingType = JPH::ESwingType::Pyramid;

	if (p_jolt_body_a == nullptr) {
		return constraint_settings.Create(JPH::Body::sFixedToWorld, *p_jolt_body_b);
	} else if (p_jolt_body_b == nullptr) {
		return constraint_settings.Create(*p_jolt_body_a, JPH::Body::sFixedToWorld);
	} else {
		return constraint_settings.Create(*p_jolt_body_a, *p_jolt_body_b);
	}
}

void JoltGeneric6DOFJointImpl3D::_update_limit_spring_parameters(int32_t p_axis) {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	QUIET_FAIL_NULL(constraint);

	JPH::SpringSettings settings = constraint->GetLimitsSpringSettings((JoltAxis)p_axis);

	settings.mMode = JPH::ESpringMode::FrequencyAndDamping;

	if (limit_spring_enabled[p_axis]) {
		settings.mFrequency = (float)limit_spring_frequency[p_axis];
		settings.mDamping = (float)limit_spring_damping[p_axis];
	} else {
		settings.mFrequency = 0.0f;
		settings.mDamping = 0.0f;
	}

	constraint->SetLimitsSpringSettings((JoltAxis)p_axis, settings);
}

void JoltGeneric6DOFJointImpl3D::_update_motor_state(int32_t p_axis) {
	if (auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr())) {
		if (motor_enabled[p_axis]) {
			constraint->SetMotorState((JoltAxis)p_axis, JPH::EMotorState::Velocity);
		} else if (spring_enabled[p_axis]) {
			constraint->SetMotorState((JoltAxis)p_axis, JPH::EMotorState::Position);
		} else {
			constraint->SetMotorState((JoltAxis)p_axis, JPH::EMotorState::Off);
		}
	}
}

void JoltGeneric6DOFJointImpl3D::_update_motor_velocity(int32_t p_axis) {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	QUIET_FAIL_NULL(constraint);

	if (p_axis >= AXIS_LINEAR_X && p_axis <= AXIS_LINEAR_Z) {
		constraint->SetTargetVelocityCS(
			{(float)motor_speed[AXIS_LINEAR_X],
			 (float)motor_speed[AXIS_LINEAR_Y],
			 (float)motor_speed[AXIS_LINEAR_Z]}
		);
	} else {
		// NOTE(mihe): We flip the direction since Jolt is CCW but Godot is CW.
		constraint->SetTargetAngularVelocityCS(
			{(float)-motor_speed[AXIS_ANGULAR_X],
			 (float)-motor_speed[AXIS_ANGULAR_Y],
			 (float)-motor_speed[AXIS_ANGULAR_Z]}
		);
	}
}

void JoltGeneric6DOFJointImpl3D::_update_motor_limit(int32_t p_axis) {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	QUIET_FAIL_NULL(constraint);

	JPH::MotorSettings& motor_settings = constraint->GetMotorSettings((JoltAxis)p_axis);

	float limit = FLT_MAX;

	if (motor_enabled[p_axis]) {
		limit = (float)motor_limit[p_axis];
	} else if (spring_enabled[p_axis]) {
		limit = (float)spring_limit[p_axis];
	}

	if (p_axis >= AXIS_LINEAR_X && p_axis <= AXIS_LINEAR_Z) {
		motor_settings.SetForceLimit(limit);
	} else {
		motor_settings.SetTorqueLimit(limit);
	}
}

void JoltGeneric6DOFJointImpl3D::_update_spring_parameters(int32_t p_axis) {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	QUIET_FAIL_NULL(constraint);

	JPH::MotorSettings& motor_settings = constraint->GetMotorSettings((JoltAxis)p_axis);

	if (spring_use_frequency[p_axis]) {
		motor_settings.mSpringSettings.mMode = JPH::ESpringMode::FrequencyAndDamping;
		motor_settings.mSpringSettings.mFrequency = (float)spring_frequency[p_axis];
	} else {
		motor_settings.mSpringSettings.mMode = JPH::ESpringMode::StiffnessAndDamping;
		motor_settings.mSpringSettings.mStiffness = (float)spring_stiffness[p_axis];
	}

	motor_settings.mSpringSettings.mDamping = (float)spring_damping[p_axis];
}

void JoltGeneric6DOFJointImpl3D::_update_spring_equilibrium(int32_t p_axis) {
	auto* constraint = static_cast<JPH::SixDOFConstraint*>(jolt_ref.GetPtr());
	QUIET_FAIL_NULL(constraint);

	if (p_axis >= AXIS_LINEAR_X && p_axis <= AXIS_LINEAR_Z) {
		const Vector3 target_position = Vector3(
			(float)spring_equilibrium[AXIS_LINEAR_X],
			(float)spring_equilibrium[AXIS_LINEAR_Y],
			(float)spring_equilibrium[AXIS_LINEAR_Z]
		);

		constraint->SetTargetPositionCS(to_jolt(target_position));
	} else {
		// NOTE(mihe): We flip the direction since Jolt is CCW but Godot is CW.
		const Basis target_orientation = Basis::from_euler(
			{(float)-spring_equilibrium[AXIS_ANGULAR_X],
			 (float)-spring_equilibrium[AXIS_ANGULAR_Y],
			 (float)-spring_equilibrium[AXIS_ANGULAR_Z]},
			EULER_ORDER_ZYX
		);

		constraint->SetTargetOrientationCS(to_jolt(target_orientation));
	}
}

void JoltGeneric6DOFJointImpl3D::_limits_changed() {
	rebuild();
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_limit_spring_parameters_changed(int32_t p_axis) {
	_update_limit_spring_parameters(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_motor_state_changed(int32_t p_axis) {
	_update_motor_state(p_axis);
	_update_motor_limit(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_motor_speed_changed(int32_t p_axis) {
	_update_motor_velocity(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_motor_limit_changed(int32_t p_axis) {
	_update_motor_limit(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_spring_state_changed(int32_t p_axis) {
	_update_motor_state(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_spring_parameters_changed(int32_t p_axis) {
	_update_spring_parameters(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_spring_equilibrium_changed(int32_t p_axis) {
	_update_spring_equilibrium(p_axis);
	_wake_up_bodies();
}

void JoltGeneric6DOFJointImpl3D::_spring_limit_changed(int32_t p_axis) {
	_update_motor_limit(p_axis);
	_wake_up_bodies();
}
