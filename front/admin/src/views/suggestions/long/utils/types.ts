interface FormItemProps {
  id?: number;
}
interface FormProps {
  formInline: FormItemProps;
}

interface RoleFormItemProps {}
interface RoleFormProps {
  formInline: RoleFormItemProps;
}

export type { FormItemProps, FormProps, RoleFormItemProps, RoleFormProps };
