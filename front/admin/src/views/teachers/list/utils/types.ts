interface FormItemProps {
  teacher_id?: number;
  /** 用于判断是`新增`还是`修改` */
  title: string;
  name: string;
  subject: string;
  grade: string;
  class: string;
  email: string;
}
interface FormProps {
  formInline: FormItemProps;
}

interface RoleFormItemProps {
  teacher_id?: number;
  name: string;
  subject: string;
  grade: string;
  class: string;
  email: string;
}
interface RoleFormProps {
  formInline: RoleFormItemProps;
}

export type { FormItemProps, FormProps, RoleFormItemProps, RoleFormProps };
