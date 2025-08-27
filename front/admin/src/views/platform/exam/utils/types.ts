interface FormItemProps {
  question_id?: number;
  /** 用于判断是`新增`还是`修改` */
  title: string;
  question: string;
  answer: string;
  options: { A: string; B: string; C: string; D: string };
  type: number;
}
interface FormProps {
  formInline: FormItemProps;
}

interface RoleFormItemProps {
  question_id?: number;
  question: string;
  options: { A: string; B: string; C: string; D: string };
  answer: string;
  type: number;
}
interface RoleFormProps {
  formInline: RoleFormItemProps;
}

export type { FormItemProps, FormProps, RoleFormItemProps, RoleFormProps };
